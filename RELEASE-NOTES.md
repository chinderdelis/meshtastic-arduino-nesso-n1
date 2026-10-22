# Release notes

## v2.8.1-beta — Arduino Nesso N1 (BLE 0.2)

**Status: BETA.** Second public, pre-release build of the Meshtastic port for
the Arduino Nesso N1. This build focuses on Bluetooth reliability and is
reported as noticeably more stable in testing, including on Android.

- Upstream base: Meshtastic firmware **v2.8.1** (`v2.8.1.8e6a88d`)
- Board variant: `arduino-nesso-n1` (ESP32-C6, 8 MB flash)

### What changed

- **2M PHY + Data Length Extension (DLE)**: on every ESP32-C6 connection the
  BLE stack now requests PHY 2M and a 251-byte data length. The old Pixel
  8-era `#if 0` workaround is removed.
- **Clean BLE disconnect before reboot**: `Power::reboot()` now tears down the
  BLE link (and releases any pending read wait) before `ESP.restart()`. The
  phone sees a real disconnect, which fixes stale links and unreliable
  re-pairing after a settings save / reboot (mostly on Android).
- **Controller 2M PHY enabled**: `CONFIG_BT_NIMBLE_LL_CFG_FEAT_LE_2M_PHY=y`
  through the variant `custom_sdkconfig`.

Result: app commands feel faster, saving settings and rebooting is quicker,
and re-pairing after reboot is reliable.

### Testing

- Build verified: `CONFIG_BT_NIMBLE_LL_CFG_FEAT_LE_2M_PHY 1` is present in the
  final `sdkconfig.h`; the app region of the merged 8 MB image is byte-for-byte
  identical to the `.app.bin`.
- Bluetooth tested and reported more stable on both Android and iOS.
- After flashing, **forget/remove the device from the phone's Bluetooth list
  and pair again**: bonds created by the alpha firmware are the usual cause of
  odd BLE behaviour after an update.

### Known issues

- **Bluetooth is still beta**: stable in testing, but unusual phone/OS
  combinations may still expose edge cases.
- The ~7 second countdown before a reboot is upstream Meshtastic behaviour
  (`DEFAULT_REBOOT_SECONDS`), not a bug of this port.
- Some peripheral connections and functions are missing or unverified.
- Environmental sensor support is limited by I2C address conflicts (M5Stack
  Unit ENV III is not compatible).
- Beta quality: expect rough edges and breaking changes between builds.

### Release assets

| File | Size | MD5 | SHA-256 |
| --- | --- | --- | --- |
| `NessoN1-Meshtastic-2.8.1-beta-8MB.bin` | 8388608 | `b6f89daf8ad9cbe5d66cdb38dd016a22` | `c30e9736fa9236d743d0bd936f3fccb2dc2539f245e99905fc38993e6b96cec1` |
| `NessoN1-Meshtastic-2.8.1-beta.factory.bin` | 2775712 | `8e5b04ed3bda87732d6085309fc1a478` | `3b19e54e5f0daebf0b8e895892ae8ad9fd48dc2cf3a9affbb93488af4b4578c6` |
| `NessoN1-Meshtastic-2.8.1-beta.app.bin` | 2710176 | `6550b9b88ac179210529a69c824a4877` | `e9f6ec89f9880f726c5fa317926bb2a2ec93f03556a6484413dbe05cc0a4b4f5` |
| `NessoN1-Meshtastic-2.8.1-beta.littlefs.bin` | 1572864 | `96595993ad073851a1d22f8d9301b63c` | `c2b08098249d64098ff30c6e68ce4d50b768f7a8676a592e346622a048679ff5` |
| `NessoN1-Meshtastic-2.8.1-beta.mt.json` | 1805 | `c227302a7d574494af6149d63e4324fa` | `9a9c472136ba019677f4ab8c3b9c9538ccc4cae7b66853c135c51f1b36d5b919` |

Also attached: `SHA256SUMS.txt`, the combined patch
`meshtastic-2.8.1-arduino-nesso-n1-ble0.2.patch` and the incremental BLE 0.2
patch `meshtastic-2.8.1-ble0.2.patch`.

#### Which file should I use?

- **Most users:** `NessoN1-Meshtastic-2.8.1-beta-8MB.bin` — a single merged
  8 MB image to flash at `0x0`. Simplest and safest.
- **Keep current settings / quick BLE test:** `.app.bin` at `0x10000`
  (bootloader and partition table are unchanged).
- **Advanced / component flashing:**
  - `.factory.bin` — full factory-style image.
  - `.app.bin` — application partition only (e.g. OTA-style updates).
  - `.littlefs.bin` — LittleFS assets partition.

Example (merged image):

```bash
esptool --chip esp32c6 --port /dev/ttyACM0 --baud 921600 \
  write-flash 0x0 NessoN1-Meshtastic-2.8.1-beta-8MB.bin
```

Example (app only, keeps settings):

```bash
esptool --chip esp32c6 --port /dev/ttyACM0 --baud 921600 \
  write-flash 0x10000 NessoN1-Meshtastic-2.8.1-beta.app.bin
```

### Reproduction

This build corresponds to upstream `v2.8.1.8e6a88d` with
`patches/meshtastic-2.8.1-arduino-nesso-n1.patch` applied (the combined patch
already includes BLE 0.2), built with PlatformIO for the `arduino-nesso-n1`
environment. If you already applied the alpha port patch, apply only
`patches/meshtastic-2.8.1-ble0.2.patch`. See the [README](README.md) for full
build and flash instructions.

## v2.8.1-alpha — Arduino Nesso N1

**Status: ALPHA.** First public, pre-release build of the Meshtastic port for
the Arduino Nesso N1. The firmware boots and has been used for field testing,
but it is still under active development and should be treated as experimental.

- Upstream base: Meshtastic firmware **v2.8.1** (`v2.8.1.8e6a88d`)
- Board variant: `arduino-nesso-n1` (ESP32-C6, 8 MB flash)

### Highlights

- Board variant for the Arduino Nesso N1 (ESP32-C6, 8 MB).
- ST7789 TFT display in landscape orientation, with LovyanGFX.
- FT5x06 touchscreen support (polled).
- PI4IO I2C GPIO expanders for buttons, LED, and backlight.
- BQ27220 battery gauge support.
- QMP6988 pressure/temperature sensor support (experimental).
- M5Stack CardKB support via the existing I2C keyboard path.

### Known issues

- **Bluetooth / BLE is unstable**: pairing and connections are not fully
  reliable. This is the main area being improved.
- Some peripheral connections and functions are missing or unverified.
- Environmental sensor support is limited by I2C address conflicts (M5Stack
  Unit ENV III is not compatible).
- Alpha quality: expect rough edges and breaking changes between builds.

### Release assets

| File | Size | MD5 | SHA-256 |
| --- | --- | --- | --- |
| `NessoN1-Meshtastic-2.8.1-alpha-8MB.bin` | 8388608 | `eaa1b229b510c62cd4320897f42683d1` | `74b48cb9ee16bba8c87421fe52fcff230ac6ffa75fffcf8238d390a8c5b0507f` |
| `NessoN1-Meshtastic-2.8.1-alpha.factory.bin` | 2774896 | `f62591c8d01181320f46ee407c4b2341` | `98a9b042f5883b285390b9d8cc26a55148a46ec19810eee2406839cad9ac9de8` |
| `NessoN1-Meshtastic-2.8.1-alpha.app.bin` | 2709360 | `edb03373aec2057aba52d0abfd9904d6` | `a265171da2662f741b338eae7575531e918e1b903780f97256f57299f6d32df3` |
| `NessoN1-Meshtastic-2.8.1-alpha.littlefs.bin` | 1572864 | `96595993ad073851a1d22f8d9301b63c` | `c2b08098249d64098ff30c6e68ce4d50b768f7a8676a592e346622a048679ff5` |

#### Which file should I use?

- **Most users:** `NessoN1-Meshtastic-2.8.1-alpha-8MB.bin` — a single merged
  8 MB image to flash at `0x0`. Simplest and safest.
- **Advanced / component flashing:**
  - `.factory.bin` — full factory-style image.
  - `.app.bin` — application partition only (e.g. OTA-style updates).
  - `.littlefs.bin` — LittleFS assets partition.

Example (merged image):

```bash
esptool --chip esp32c6 --port /dev/ttyACM0 --baud 921600 \
  write-flash 0x0 NessoN1-Meshtastic-2.8.1-alpha-8MB.bin
```

### Reproduction

This build corresponds to upstream `v2.8.1.8e6a88d` with
`patches/meshtastic-2.8.1-arduino-nesso-n1.patch` applied, built with
PlatformIO for the `arduino-nesso-n1` environment. See the
[README](README.md) for full build and flash instructions.
