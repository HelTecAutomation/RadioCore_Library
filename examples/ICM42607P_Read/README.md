# ICM42607P Read

## Purpose

This example reads the onboard ICM42607P accelerometer and gyroscope on a RadioCore RC32 or RC52 and prints both three-axis measurements in physical units ten times per second.

## Requirements

- RadioCore RC32 or RC52 board configuration
- [TDK InvenSense ICM42670P](https://github.com/tdk-invn-oss/motion.arduino.ICM42670P) version 1.0.8 or newer

The physical sensor is an ICM42607P. The ICM42670P library is a compatible driver dependency used by the reference firmware; it is not the detected sensor model. The dependency is declared in `library.properties` and can be installed automatically by dependency-aware Arduino tools.

## Wiring and board differences

The ICM42607P is integrated into the supported boards, so no external wiring is required.

| Board | Sensor I2C SDA | Sensor I2C SCL | Sensor power control | Sensor reset |
| --- | ---: | ---: | --- | --- |
| Heltec RC32 | GPIO21 | GPIO18 | GPIO46, active high | GPIO2, high to release |
| Heltec RC52 | P1.11 (43) | P0.02 (2) | P0.12 (12), active high | P1.15 (47), high to release |

Both boards wait 100 ms after enabling the Sensor power rail and releasing reset before starting I2C. The sketch detects the ICM42607P at `0x68` or `0x69` and verifies that register `0x75` contains its `0x60` identity value.

## Behavior

After identity verification, the sketch accepts driver initialization status `0` or `-3`. Version 1.0.8 of the ICM42670P driver returns `-3` because it expects the ICM42670P identity value, while the ICM42607P has a compatible register map and reports `0x60`. Other initialization errors stop the example.

The accelerometer runs at 50 Hz with a +/-2 g full-scale range, and the gyroscope runs at 50 Hz with a +/-2000 degrees-per-second full-scale range. Every 100 ms, the sketch polls the latest event, converts signed 16-bit readings with `raw * full_scale / 32768`, and prints acceleration in g and angular rate in degrees per second at 115200 baud.

## Limitations

The values use the configured full-scale ranges but do not include offset calibration or temperature compensation. The example does not read temperature, calculate orientation, or enable wake-on-motion.

The board Sensor interrupt is deliberately unused because RC32 and RC52 share that signal with the onboard rotary-encoder expander. The example intentionally supports only the onboard ICM42607P configurations declared by RC32 and RC52.
