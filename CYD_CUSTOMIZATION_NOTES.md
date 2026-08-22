# ESP32-2432S028 CYD customization notes

CYD-specific development is maintained on the `cyd-custom` branch. These notes record the tested hardware, build configuration, BLE changes, and open investigation items without changing the upstream defaults for other boards.

## Hardware and known-good build

The tested target is the ESP32-2432S028 Cheap Yellow Display with an ESP32-D0WD-V3 revision 3.1 and a CH340 USB serial interface. Its physical SPI flash is **4 MB**. Two tested boards used compatible flash chips: manufacturer IDs `0x5E` and `0x14`, both with device ID `0x4016`. The same corrected firmware worked on both.

The first CYD attempt used `FlashSize=16M` and was uploaded with `--flash_size 16MB`, even though the physical devices have 4 MB and the partition table already ended at `0x400000`. That mismatch produced a blank display. The working configuration is:

- `FlashSize=4M`
- `PartitionScheme=min_spiffs`
- `PSRAM=disabled`
- `UploadSpeed=921600`
- DIO flash mode at 80 MHz
- 4 MB image headers
- partition maximum `0x400000`

Correcting the flash declaration restored the display. The application typically uses approximately 1.83 MB, or 93%, of the available app partition, so future features have limited code-size headroom.

## Freeze investigation

One CYD froze during operation, but no root cause was proven. Read-only inspection found:

- a blank coredump partition at `0x3F0000`–`0x400000`
- blank SPIFFS
- ordinary configuration in NVS
- no persistent crash history, runtime log, or reset-history mechanism
- classic ESP32 brownout detection disabled
- source comments acknowledging SD, SPI, watchdog, and heap-related risks

The recommended future capture method is a receive-only UART0 logger at 115200 8-N-1. UART0 TX is GPIO1 and RX is GPIO3. Connect only `CYD TX -> logger RX` and `CYD GND -> logger GND`; do not connect logger TX, DTR, RTS, or VCC.

## BLE randomized-MAC false positive

`MAC Spoofing Suspected` persisted with nearby devices powered down, while the CYD was battery powered, away from the house, and after active scanning was changed experimentally to passive scanning. Passive scanning did not resolve it.

The actual defect incremented `macChangeCount` whenever another advertisement arrived from the same already-known randomized MAC. Normal repeated advertising therefore deterministically crossed the warning threshold even though no MAC change had occurred. The fix removes that increment without inventing a replacement correlation algorithm; other detectors remain unchanged.

## Sniffer Pause / Resume

The Sniffer updates quickly enough to make MAC addresses, RSSI values, names, and alerts difficult to read. The bottom-right navigation slot now pauses active NimBLE scanning and freezes result processing, scan rotation, timeout cleanup, and display mutation while preserving devices, counts, alerts, and suspicious state.

Resume shifts each device's `lastSeen` timestamp by the pause duration before restarting scanning. A pause longer than the 30-second timeout therefore does not immediately purge the preserved table.

## Nearby-device RSSI admission

Physical testing observed advertisements around `-97 dBm`, well outside the owner's desired immediate surroundings. New BLE devices are therefore admitted only when first observed at `-65 dBm` or stronger. Weaker unknown devices are rejected before consuming a table slot or affecting new-device/flood accounting. Once admitted, a device continues to be tracked through later RSSI fading and retains normal timeout behavior.

This is an **RF admission/display preference**, not a security or jamming judgment.

## Remaining BLE investigations

### Device-table capacity

The fixed table has `MAX_DEVICES=32`, so dense BLE environments can produce `Max devices reached!` quickly. Do not automatically copy an ESP32-S3-oriented suggestion to raise it to 128: this CYD uses a classic ESP32 with tighter memory and application-partition constraints.

### Jamming heuristic

The current detector reports `Jamming Suspected` when either cumulative `packetCount > 20` or BLE RSSI is stronger than `-20 dBm`. Cumulative packet count is not a packet-rate measurement, so ordinary repeated advertising can eventually satisfy it. This remains unresolved and was not changed with the RSSI admission filter.

### Classic Bluetooth discrepancy

The source has a `startBTScan()` state/display path, but inspection found that it does not initiate ESP-IDF Classic Bluetooth discovery. BLE initialization also releases Classic controller memory. Referencing a Classic-BT cancellation API during Pause development linked the otherwise-unused Classic stack and grew the application to approximately 121% of its partition, so that call was removed.

Physical display testing nevertheless appeared to show entries identified as `BT`, including around `-97 dBm`. The discrepancy remains unresolved; additional Classic-BT APIs are intentionally not enabled.
