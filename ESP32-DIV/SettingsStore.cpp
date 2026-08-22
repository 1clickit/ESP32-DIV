#include "SettingsStore.h"
#if BOARD_HAS_ESP32S3
#include <ArduinoJson.h>
#include <SD.h>
#else
#include <Preferences.h>
#endif
#include "utils.h"


static AppSettings g_settings;
AppSettings& settings() { return g_settings; }

static const AccentOption kAccentPresets[] = {
  {"Orange", 0xFBE4},
  {"Green",  0xB721},
  {"Red",    0xF800},
  {"Cyan",   0x07FF},
  {"Purple", 0xF81F},
  {"Yellow", 0xFFE0},
  {"White",  0xFFFF},
};

uint8_t accentPresetClamp(uint8_t preset) {
  if (preset >= ACCENT_PRESET_COUNT) return 0;
  return preset;
}

uint16_t accentColor565(uint8_t preset) {
  return kAccentPresets[accentPresetClamp(preset)].color565;
}

const char* accentPresetName(uint8_t preset) {
  return kAccentPresets[accentPresetClamp(preset)].name;
}

const char* settingsBoardProfileId() {
  return TOUCH_PROFILE_ID;
}

void settingsApplyBoardTouchDefaults() {
  auto& s = g_settings;
  s.touchXMin = TOUCH_X_MIN;
  s.touchXMax = TOUCH_X_MAX;
  s.touchYMin = TOUCH_Y_MIN;
  s.touchYMax = TOUCH_Y_MAX;
}

#if BOARD_HAS_ESP32S3

static bool settingsTouchSavedForBoard(const StaticJsonDocument<512>& doc) {
  JsonObjectConst touch = doc["touch"];
  if (touch.isNull()) {
    return false;
  }
  if (!touch["xMin"].is<uint16_t>() || !touch["xMax"].is<uint16_t>() ||
      !touch["yMin"].is<uint16_t>() || !touch["yMax"].is<uint16_t>()) {
    return false;
  }
  const char* savedBoard = doc["board"] | "";
  if (savedBoard[0] == '\0') {
    return true;
  }
  return strcmp(savedBoard, TOUCH_PROFILE_ID) == 0;
}

static bool sd_mounted = false;
static bool mountSD() {

  if (sd_mounted) {
    if (SD.cardType() != CARD_NONE) return true;
    sd_mounted = false;
  }

  sd_mounted = isSDCardAvailable();
  return sd_mounted;
}

static bool ensureDir(const char* dirPath) {
  if (!mountSD()) return false;
  if (!SD.exists(dirPath)) {
    if (SD.mkdir(dirPath)) return true;

    if (dirPath && dirPath[0] == '/') {
      return SD.mkdir(dirPath + 1);
    }
    return false;
  }
  return true;
}

bool settingsLoad() {
  settingsApplyBoardTouchDefaults();
  sdRetryMount();
  if (!mountSD()) return false;
  if (!SD.exists(SETTINGS_PATH)) return true;

  File f = SD.open(SETTINGS_PATH, FILE_READ);
  if (!f) return false;

  StaticJsonDocument<512> doc;
  DeserializationError err = deserializeJson(doc, f);
  f.close();
  if (err) return false;

  auto& s = g_settings;
  s.brightness      = doc["brightness"]      | s.brightness;
  s.theme           = (Theme)(uint8_t)(doc["theme"] | (uint8_t)s.theme);
  s.accentColor     = accentPresetClamp(doc["accentColor"] | s.accentColor);
  s.neopixelEnabled = doc["neopixelEnabled"] | s.neopixelEnabled;

  s.autoWifiScan    = doc["autoWifiScan"]    | s.autoWifiScan;
  s.autoBleScan     = doc["autoBleScan"]     | s.autoBleScan;

  if (s.autoWifiScan != s.autoBleScan) {
    bool en = (s.autoWifiScan || s.autoBleScan);
    s.autoWifiScan = en;
    s.autoBleScan  = en;
  }

  if (settingsTouchSavedForBoard(doc)) {
    JsonObjectConst touch = doc["touch"];
    s.touchXMin = touch["xMin"] | s.touchXMin;
    s.touchXMax = touch["xMax"] | s.touchXMax;
    s.touchYMin = touch["yMin"] | s.touchYMin;
    s.touchYMax = touch["yMax"] | s.touchYMax;
  } else {
    settingsApplyBoardTouchDefaults();
  }

  return true;
}

bool settingsSave() {
  sdRetryMount();

  if (!ensureDir("/config")) {
    sd_mounted = false;
    if (!ensureDir("/config")) return false;
  }

  File f = SD.open(SETTINGS_PATH, FILE_WRITE);
  if (!f) {
    sd_mounted = false;
    if (!mountSD()) return false;
    f = SD.open(SETTINGS_PATH, FILE_WRITE);
    if (!f) return false;
  }

  auto& s = g_settings;
  StaticJsonDocument<512> doc;
  doc["board"]           = TOUCH_PROFILE_ID;
  doc["brightness"]      = s.brightness;
  doc["theme"]           = (uint8_t)s.theme;
  doc["accentColor"]     = s.accentColor;
  doc["neopixelEnabled"] = s.neopixelEnabled;

  doc["autoWifiScan"]    = s.autoWifiScan;
  doc["autoBleScan"]     = s.autoBleScan;

  JsonObject t = doc.createNestedObject("touch");
  t["xMin"] = s.touchXMin;
  t["xMax"] = s.touchXMax;
  t["yMin"] = s.touchYMin;
  t["yMax"] = s.touchYMax;

  bool ok = serializeJson(doc, f) > 0;
  f.close();
  return ok;
}

#else

static constexpr char kSettingsNamespace[] = "settings";
static constexpr uint8_t kSettingsSchemaVersion = 1;

static bool settingsTouchAxisValid(uint16_t first, uint16_t second) {
  return first <= 4095 && second <= 4095 && first != second;
}

bool settingsLoad() {
  g_settings = AppSettings{};
  settingsApplyBoardTouchDefaults();

  Preferences prefs;
  if (!prefs.begin(kSettingsNamespace, true)) {
    return true;
  }

  if (prefs.getUChar("ver", 0) != kSettingsSchemaVersion) {
    prefs.end();
    return true;
  }

  auto& s = g_settings;
  s.brightness = prefs.getUChar("bright", s.brightness);

  uint8_t savedTheme = prefs.getUChar("theme", (uint8_t)s.theme);
  if (savedTheme <= (uint8_t)Theme::Light) {
    s.theme = (Theme)savedTheme;
  }

  s.accentColor = accentPresetClamp(prefs.getUChar("accent", s.accentColor));
  s.neopixelEnabled = prefs.getBool("neo", s.neopixelEnabled);
  s.autoWifiScan = prefs.getBool("wifi", s.autoWifiScan);
  s.autoBleScan = prefs.getBool("ble", s.autoBleScan);

  if (s.autoWifiScan != s.autoBleScan) {
    bool enabled = s.autoWifiScan || s.autoBleScan;
    s.autoWifiScan = enabled;
    s.autoBleScan = enabled;
  }

  String savedBoard = prefs.getString("board", "");
  if (savedBoard == TOUCH_PROFILE_ID) {
    uint16_t xMin = prefs.getUShort("txmin", TOUCH_X_MIN);
    uint16_t xMax = prefs.getUShort("txmax", TOUCH_X_MAX);
    uint16_t yMin = prefs.getUShort("tymin", TOUCH_Y_MIN);
    uint16_t yMax = prefs.getUShort("tymax", TOUCH_Y_MAX);
    if (settingsTouchAxisValid(xMin, xMax) && settingsTouchAxisValid(yMin, yMax)) {
      s.touchXMin = xMin;
      s.touchXMax = xMax;
      s.touchYMin = yMin;
      s.touchYMax = yMax;
    }
  }

  prefs.end();
  return true;
}

bool settingsSave() {
  Preferences prefs;
  if (!prefs.begin(kSettingsNamespace, false)) {
    return false;
  }

  auto& s = g_settings;
  bool ok = prefs.putUChar("ver", 0) == sizeof(uint8_t);
  ok = ok && prefs.putString("board", TOUCH_PROFILE_ID) > 0;
  ok = ok && prefs.putUChar("bright", s.brightness) == sizeof(uint8_t);
  ok = ok && prefs.putUChar("theme", (uint8_t)s.theme) == sizeof(uint8_t);
  ok = ok && prefs.putUChar("accent", accentPresetClamp(s.accentColor)) == sizeof(uint8_t);
  ok = ok && prefs.putBool("neo", s.neopixelEnabled) == sizeof(uint8_t);
  ok = ok && prefs.putBool("wifi", s.autoWifiScan) == sizeof(uint8_t);
  ok = ok && prefs.putBool("ble", s.autoBleScan) == sizeof(uint8_t);
  ok = ok && prefs.putUShort("txmin", s.touchXMin) == sizeof(uint16_t);
  ok = ok && prefs.putUShort("txmax", s.touchXMax) == sizeof(uint16_t);
  ok = ok && prefs.putUShort("tymin", s.touchYMin) == sizeof(uint16_t);
  ok = ok && prefs.putUShort("tymax", s.touchYMax) == sizeof(uint16_t);
  if (ok) {
    ok = prefs.putUChar("ver", kSettingsSchemaVersion) == sizeof(uint8_t);
  }

  prefs.end();
  return ok;
}

#endif
