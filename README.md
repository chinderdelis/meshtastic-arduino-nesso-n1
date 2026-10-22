# Meshtastic for Arduino Nesso N1

Community firmware port of [Meshtastic](https://meshtastic.org/) for the
**Arduino Nesso N1** (ESP32-C6).

> **Status: BETA** — the firmware boots and is used for daily testing. The
> BLE 0.2 fixes make Bluetooth **noticeably more stable**, including on
> Android, but the port is still under active testing. Some peripherals are
> missing or only partially verified (see [Known issues](#known-issues)).
>
> This is an unofficial, community-maintained port. It is **not** affiliated
> with, sponsored by, or endorsed by Meshtastic or Arduino. Use at your own
> risk.

## Overview

This repository contains the board support needed to build Meshtastic for the
Arduino Nesso N1, on top of the upstream firmware:

- Upstream base: Meshtastic firmware **v2.8.1** (tag `v2.8.1.8e6a88d`)
- Board variant: `arduino-nesso-n1`
- MCU: **ESP32-C6**, 8 MB flash, Bluetooth (BLE) enabled

The changes are provided as a patch against upstream (`patches/`), plus a copy
of the board variant sources (`variants/`) for reference.

## What's new in beta (BLE 0.2)

This is the second public build and focuses on Bluetooth reliability:

- **2M PHY and Data Length Extension (DLE)** are now requested on every
  ESP32-C6 connection. The old Pixel 8-era workaround (`#if 0`) is gone, so
  the link negotiates larger packets and a faster PHY.
- **Clean BLE disconnect before reboot.** `Power::reboot()` now tears down the
  BLE link and waits briefly before `ESP.restart()`, so the phone sees a real
  disconnect instead of a stale link. This fixes the "device never re-pairs
  after a settings save / reboot" behaviour, mostly on Android.
- **2M PHY enabled in the controller** through
  `CONFIG_BT_NIMBLE_LL_CFG_FEAT_LE_2M_PHY=y`, so the requested PHY is actually
  available at the NimBLE/controller level.

Result in testing: app commands feel faster, saving settings and rebooting is
quicker, and re-pairing after reboot is reliable. Bluetooth has been tested
and reported as more stable on both Android and iOS.

## Hardware support

| Feature | Status | Notes |
| --- | --- | --- |
| ESP32-C6 MCU | Working | |
| SX1262 LoRa radio | Working | WaveShare Core1262-868M class SPI module |
| ST7789 TFT display | Working | Landscape, 240×135 |
| Touchscreen (FT5x06, `0x38`) | Working | Polled; shares IRQ line with the system |
| Buttons (PI4IO I2C expanders) | Working | KEY1 / KEY2 |
| Battery gauge (BQ27220) | Working | |
| Charger init (AW32001E) | Working | Init only |
| QMP6988 pressure/temperature | Experimental | via Grove/I2C |
| M5Stack CardKB | Working | existing I2C keyboard path |
| Bluetooth (BLE) | Working (beta) | 2M PHY + DLE; stable in testing, see above |
| GPS | Not present | |

## Known issues

- **Bluetooth / BLE is still beta**: it is now stable in testing, but unusual
  phone/OS combinations may still expose edge cases. Please report them.
- The ~7 second countdown before a reboot is upstream Meshtastic behaviour
  (`DEFAULT_REBOOT_SECONDS`), not a bug of this port.
- Some peripheral connections and functions are still unverified or missing.
- Environmental sensor support is limited by I2C address conflicts (see below).
- Beta quality: expect rough edges and breaking changes between builds.

## Quick start (prebuilt image)

Download the 8 MB merged image from the [latest beta release](../../releases)
and flash it at offset `0x0`:

```bash
esptool --chip esp32c6 --port /dev/ttyACM0 --baud 921600 \
  write-flash 0x0 NessoN1-Meshtastic-2.8.1-beta-8MB.bin
```

Optional, for a clean start (erases settings/NVS):

```bash
esptool --chip esp32c6 --port /dev/ttyACM0 erase-flash
```

Then reset / re-plug the USB cable. If the serial port does not appear, hold
**BOOT**, press and release **RESET**, then release **BOOT**, and retry.

> Flashing the merged image at `0x0` resets the configuration (factory
> settings). The image does **not** contain any personal configuration, WiFi
> credentials, or keys — only bootloader, partitions, application, and the
> LittleFS assets.

### Upgrading from the alpha build

After flashing, **forget/remove the device from your phone's Bluetooth list
and pair again**. Old bonds from the alpha firmware are the most common cause
of odd BLE behaviour after an update.

If you only want to test the Bluetooth changes and keep your current settings,
flash the application partition instead (bootloader and partition table are
unchanged):

```bash
esptool --chip esp32c6 --port /dev/ttyACM0 --baud 921600 \
  write-flash 0x10000 NessoN1-Meshtastic-2.8.1-beta.app.bin
```

## Build from source

1. Clone the upstream firmware and check out the base tag:

   ```bash
   git clone https://github.com/meshtastic/firmware.git
   cd firmware
   git checkout v2.8.1.8e6a88d
   ```

2. Apply the combined patch:

   ```bash
   git apply /path/to/patches/meshtastic-2.8.1-arduino-nesso-n1.patch
   ```

   (Alternatively, copy `variants/esp32c6/arduino_nesso_n1/` into the tree and
   apply only the `src/` changes; the combined patch already includes both.)

   If you already applied the alpha port patch, apply only the BLE 0.2 delta
   on top of it:

   ```bash
   git apply /path/to/patches/meshtastic-2.8.1-ble0.2.patch
   ```

3. Build the variant with PlatformIO:

   ```bash
   pio run -e arduino-nesso-n1
   ```

   The build target is `arduino-nesso-n1` (ESP32-C6, 8 MB, BLE on, LovyanGFX,
   BQ27220). The touchscreen can be disabled at build time with
   `-D HAS_TOUCHSCREEN=0`.

### Build troubleshooting

If the build stops with `ModuleNotFoundError: No module named
'SCons.Tool.FortranCommon'` (or `can't open file '.../tool-scons/scons.py'`),
the PlatformIO Core `tool-scons` version does not match the version expected
by the pioarduino ESP32 platform (SCons 4.8.1). With PlatformIO Core 6.2.0 the
requirement is read from the uv-cached copy of PlatformIO, so align **both**
copies of `platformio/dependencies.py` to the pioarduino 4.8.1 package:

```text
~/.local/share/pipx/venvs/platformio/lib/python3.12/site-packages/platformio/dependencies.py
~/.platformio/.cache/uv/archive-v0/*/platformio/dependencies.py
```

Use `tool-scons` = `~4.40801.0`, or point it directly at
`https://github.com/pioarduino/registry/releases/download/0.0.1/scons-4.8.1.zip`.

Also note that a stale `sdkconfig.<env>` file wins over `sdkconfig.defaults`:
if a key is already present as `# CONFIG_... is not set`, change it to `=y`
(or delete the file) before rebuilding the IDF libraries.

## Technical notes

### Bluetooth (BLE)

- On ESP32-C6, `NimbleBluetooth::onConnect()` requests 2M PHY
  (`ble_gap_set_prefered_le_phy`) and DLE 251 bytes
  (`ble_gap_set_data_len`). Both calls are logged at info/warn level.
- `NimbleBluetooth::prepareForReboot()` (called from `Power::reboot()` through
  `prepareBleForReboot()`) marks the stack as de-initialising, releases any
  pending BLE read wait, disconnects the current connection and waits 250 ms
  before the reset.
- The controller is built with `CONFIG_BT_NIMBLE_LL_CFG_FEAT_LE_2M_PHY=y` via
  the variant's `custom_sdkconfig`.

### Display

The board uses an ST7789 panel driven through LovyanGFX in landscape
orientation (`TFT_HEIGHT 240`, `TFT_WIDTH 135`, offsets `X -52`, `Y 40`,
`TFT_OFFSET_ROTATION 2`).

### I2C addresses

- The touchscreen uses address `0x38`, which is also the default AHT10 address
  in Meshtastic. For this board, `0x38` is ignored by the AHT10 auto-detection
  path so the touch controller is not misdetected as an environment sensor.
- The onboard PI4IO expanders use `0x43` and `0x44`.
- CardKB through the Grove/I2C port uses the existing Meshtastic I2C keyboard
  path and needs no board-specific driver.
- **M5Stack Unit ENV III is not compatible** over the Qwiic/Grove port: the
  onboard expander already uses `0x44`, which is also the SHT30 address used by
  ENV III, so the two cannot share the bus. QMP6988 pressure readings are
  supported independently.

### LoRa

The board has been validated with the LoRa frequency slot set explicitly in the
app. A mismatch between the configured slot and the other mesh nodes can look
like successful local TX with no network join.

## Repository layout

```text
patches/
  meshtastic-2.8.1-arduino-nesso-n1.patch  Combined port + BLE 0.2 (all changed files)
  meshtastic-2.8.1-ble0.2.patch            BLE 0.2 delta, for the alpha port patch
variants/                                  Board variant sources (pins, display, touch, GPIO expander, README)
```

## Credits

- [Meshtastic firmware](https://github.com/meshtastic/firmware) — upstream
  project and firmware.
- [Arduino](https://www.arduino.cc/) — Nesso N1 hardware.
- LovyanGFX, RadioLib and the other open-source dependencies used by Meshtastic.

## License

This project follows the upstream Meshtastic firmware license: **GPL-3.0**
(see [LICENSE](LICENSE)).
