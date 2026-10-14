# Arduino Nesso N1 Support Notes

This document summarizes the Nesso N1-specific firmware support and the current
known limitations.

## Supported hardware

- ESP32-C6 based Nesso N1 board variant: `arduino-nesso-n1`
- SX1262 LoRa radio
- TFT display
- Touchscreen, enabled by default
- M5Stack CardKB through the existing I2C keyboard path
- Environmental telemetry through the Grove/I2C port, for sensors already
  supported by Meshtastic

## Known-good baseline

For stable field testing, use the 2.7-based Nesso N1 branch state at:

```text
263e1c6f2da1a72f6a302e6a974896d1241b9978
```

That commit is based on firmware 2.7.23 and includes the verified Nesso N1
support work, including display, touch input, CardKB, and QMP6988 telemetry.

The current 2.8-based branch is useful for development and upstream follow-up,
but should be treated as a revalidation target until the ESP32-C6 behavior is
confirmed again on hardware.

## Build options

The touchscreen can be disabled at build time:

```ini
-D HAS_TOUCHSCREEN=0
```

The default variant keeps it enabled:

```cpp
#define HAS_TOUCHSCREEN 1
```

The define is intentionally overrideable from PlatformIO build flags.

## I2C address notes

The Nesso touchscreen uses I2C address `0x38`, which is also the default AHT10
address in Meshtastic. For this board, `0x38` is ignored by the AHT10
auto-detection path to avoid registering the touchscreen as a broken AHT10
environment sensor.

CardKB uses the existing Meshtastic I2C keyboard detection and does not require a
Nesso-specific driver.

## LoRa notes

The board has been validated with the app's LoRa frequency slot set explicitly.
A mismatch between the configured slot and the other mesh nodes can look like
successful local TX with no network join.

## Environmental sensor notes

M5Stack Unit ENV and Unit ENV Pro can work through existing Meshtastic sensor
drivers when their onboard sensors are supported.

M5Stack Unit ENV III is not compatible with the Nesso N1 over the Qwiic/Grove
port. The board's onboard E1 expander already uses I2C address `0x44`, and ENV
III's SHT30 temperature/humidity sensor also uses `0x44`, so both devices cannot
share the same bus. This matches the limitation documented by Arduino for the
Nesso N1 user manual.

QMP6988 pressure readings are supported, but ENV III still remains incomplete on
Nesso N1 because the SHT30 side cannot coexist on the same bus due to the
`0x44` address conflict.
