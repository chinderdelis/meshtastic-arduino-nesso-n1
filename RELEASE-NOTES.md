# Release notes

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
