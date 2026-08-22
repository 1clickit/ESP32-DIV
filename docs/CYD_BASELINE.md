# CYD Known-Good Baseline

This document records the first controlled build and flash baseline for the Cheap Yellow Display target. It distinguishes verified build/flash facts from application behavior that still requires hardware testing.

## Repository

- Repository: `1clickit/ESP32-DIV`
- Upstream: `cifertech/ESP32-DIV`
- Local path: `C:\Users\chris\ESP32-DIV`
- Working branch: `cyd-custom`
- Upstream-derived starting commit: `9d4d82fe7a12febf554b12e1eca6d434ebe79d39`
- Baseline source change: `BOARD_CYD` enabled and `BOARD_ESP32_DIV_V2` disabled

## Hardware

- Baseline USB port: `COM4`
- USB interface: USB-SERIAL CH340
- USB VID/PID: `1A86:7523`
- USB driver: `3.8.2023.2`
- ESP32 reported by esptool: ESP32-D0WD-V3, revision 3.1
- CPU: dual-core, 240 MHz
- Crystal: 40 MHz

COM port numbering can differ between machines and must not be treated as a permanent hardware identity. Revalidate the USB interface and VID/PID before future hardware writes.

## Build environment

- Arduino CLI: `1.4.1`
- Arduino ESP32 core: `2.0.10`
- Espressif GCC: `8.4.0`
- FQBN: `esp32:esp32:esp32:FlashSize=16M,PartitionScheme=min_spiffs,PSRAM=disabled,UploadSpeed=921600`
- Result: successful, with no final build warnings or errors
- Program usage: `1,829,849 / 1,966,080 bytes (93%)`
- Static RAM: `111,460 / 327,680 bytes (34%)`

The application already consumes 93% of its partition. Future custom features have limited headroom and may eventually require code-size work or partition-layout consideration. The partition layout was not changed for this baseline.

## Compatibility dependencies

| Dependency | Version | Notes |
| --- | --- | --- |
| Arduino ESP32 core | `2.0.10` | Required core |
| TFT_eSPI | `2.5.43` | Repository-bundled/customized |
| SmartRC CC1101 | `2.5.7` | Repository-bundled/customized |
| PCF8574 | `0.4.4` | |
| XPT2046_Touchscreen | `1.4` | |
| RF24 | `1.6.0` | |
| NimBLE-Arduino | `1.4.3` | Compatibility pin |
| rc-switch | `2.6.4` | |
| arduinoFFT | `1.6.2` | Compatibility pin |
| IRremoteESP8266 | `2.9.0` | |
| ArduinoJson | `7.4.3` | |
| Adafruit PN532 | `1.3.4` | |
| Adafruit BusIO | `1.17.4` | |

NimBLE-Arduino `2.5.0` and arduinoFFT `2.0.4` were incompatible with this source. The successful build pinned NimBLE-Arduino to `1.4.3` and arduinoFFT to `1.6.2`.

## Material workstation paths

| Path | Purpose |
| --- | --- |
| `C:\Users\chris\ESP32-DIV` | Repository checkout |
| `C:\Users\chris\ESP32-DIV-build-cyd` | Successful CYD build outputs |
| `C:\Users\chris\AppData\Local\Arduino15` | Arduino package indexes, caches, downloads, cores, and tools |
| `C:\Users\chris\AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.10` | Active ESP32 core |
| `C:\Users\chris\AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.10\platform.txt` | Repository-provided platform configuration applied to the core |
| `C:\Users\chris\AppData\Local\Arduino15\packages\esp32\hardware\esp32\2.0.10\platform.txt.pre-esp32-div.bak` | Backup of the prior platform file |
| `E:\OneDrive - 1CLICK IT LLC\Documents\Arduino\libraries` | Configured Arduino sketchbook library location on this workstation |

The following directories were created during the initial sketchbook-path mismatch but were not used by the successful final build. They remain in place:

- `C:\Users\chris\Documents\Arduino\libraries\TFT_eSPI-master`
- `C:\Users\chris\Documents\Arduino\libraries\SmartRC-CC1101-Driver-Lib-master`

## Build artifacts

Artifact directory: `C:\Users\chris\ESP32-DIV-build-cyd`

| Artifact | Size |
| --- | ---: |
| `ESP32-DIV.ino.bin` | 1,835,600 bytes |
| `ESP32-DIV.ino.bootloader.bin` | 18,992 bytes |
| `ESP32-DIV.ino.partitions.bin` | 3,072 bytes |
| `boot_app0.bin` | 8,192 bytes |
| `ESP32-DIV.ino.elf` | 39,104,328 bytes |

## Flash baseline

The first controlled flash:

- Revalidated `COM4` as the expected CH340 before writing.
- Detected an ESP32-D0WD-V3 revision 3.1.
- Wrote all four required images successfully.
- Verified every image hash.
- Completed a hard reset via RTS successfully.
- Observed the same CH340 identity on `COM4` after flashing.
- Did not open serial afterward because opening the CH340 interface could toggle DTR/RTS and reset the device.

### Verified

- Build succeeded.
- Device identity succeeded.
- Flash succeeded.
- Image hashes verified.
- Reset succeeded.
- USB serial interface returned.

### Not yet verified

- Display/UI behavior
- Touchscreen behavior
- SD functionality
- Wi-Fi/BLE functionality
- Other ESP32-DIV features

Application or UI functionality must not be inferred from a successful flash alone.

## Workstation side effects

Baseline preparation made these workstation-level changes, not repository changes:

- Replaced the previously installed ESP32 core `3.3.8` with required core `2.0.10`.
- Applied the repository-provided `platform.txt`.
- Preserved the prior platform file as `platform.txt.pre-esp32-div.bak`.
- Installed and pinned the compatible libraries listed above.
