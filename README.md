# Meshtastic for Arduino Nesso N1

Community firmware port of [Meshtastic](https://meshtastic.org/) for the
**Arduino Nesso N1** (ESP32-C6).

> **Status: ALPHA** — the firmware boots and has been used for field testing,
> but it is still under active testing. Some peripherals are missing, unstable,
> or only partially verified (see [Known issues](#known-issues)).
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
| Bluetooth (BLE) | **Unstable** | connections not fully reliable, improving |
| GPS | Not present | |

## Known issues

- **Bluetooth / BLE**: pairing and connections are not fully reliable yet;
  improvements are in progress. This is the main area that needs work.
- Some peripheral connections and functions are still unverified or missing.
- Environmental sensor support is limited by I2C address conflicts (see below).
- This is an alpha build: expect rough edges and breaking changes.

## Quick start (prebuilt image)

Download the 8 MB merged image from the [latest alpha release](../../releases)
and flash it at offset `0x0`:

```bash
esptool --chip esp32c6 --port /dev/ttyACM0 --baud 921600 \
  write-flash 0x0 NessoN1-Meshtastic-2.8.1-alpha-8MB.bin
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

## Build from source

1. Clone the upstream firmware and check out the base tag:

   ```bash
   git clone https://github.com/meshtastic/firmware.git
   cd firmware
   git checkout v2.8.1.8e6a88d
   ```

2. Apply the port patch:

   ```bash
   git apply /path/to/patches/meshtastic-2.8.1-arduino-nesso-n1.patch
   ```

   (Alternatively, copy `variants/esp32c6/arduino_nesso_n1/` into the tree and
   apply only the `src/` changes; the combined patch already includes both.)

3. Build the variant with PlatformIO:

   ```bash
   pio run -e arduino-nesso-n1
   ```

   The build target is `arduino-nesso-n1` (ESP32-C6, 8 MB, BLE on, LovyanGFX,
   BQ27220). The touchscreen can be disabled at build time with
   `-D HAS_TOUCHSCREEN=0`.

### Build troubleshooting

If the build stops with `ModuleNotFoundError: No module named
'SCons.Tool.FortranCommon'`, the PlatformIO Core `tool-scons` version does not
match the version expected by the pioarduino ESP32 platform. Align the
`tool-scons` version required by PlatformIO Core with the one declared in the
platform `platform.json` (a SCons `4.8.1`-compatible Pairing is known to work).

## Technical notes

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
patches/   Patch against upstream Meshtastic v2.8.1 (all 22 changed/added files)
variants/  Board variant sources (pins, display, touch, GPIO expander, README)
```

## Credits

- [Meshtastic firmware](https://github.com/meshtastic/firmware) — upstream
  project and firmware.
- [Arduino](https://www.arduino.cc/) — Nesso N1 hardware.
- LovyanGFX, RadioLib and the other open-source dependencies used by Meshtastic.

## License

This project follows the upstream Meshtastic firmware license: **GPL-3.0**
(see [LICENSE](LICENSE)).
