# MMC5983MA Read

## Purpose

This example reads the onboard MMC5983MA three-axis magnetometer on a RadioCore RC32 or RC52 and prints the magnetic field in gauss ten times per second.

## Requirements

- RadioCore RC32 or RC52 board configuration
- [SparkFun MMC5983MA Magnetometer Arduino Library](https://github.com/sparkfun/SparkFun_MMC5983MA_Magnetometer_Arduino_Library) version 1.1.5 or newer

The dependency is declared in `library.properties` and can be installed automatically by dependency-aware Arduino tools.

## Wiring and board differences

The MMC5983MA is integrated into the supported boards, so no external wiring is required.

| Board | Sensor I2C SDA | Sensor I2C SCL | Sensor power control | Sensor reset |
| --- | ---: | ---: | --- | --- |
| Heltec RC32 | GPIO21 | GPIO18 | GPIO46, active high | GPIO2, high to release |
| Heltec RC52 | P1.11 (43) | P0.02 (2) | P0.12 (12), active high | P1.15 (47), high to release |

Both boards wait 100 ms after enabling the Sensor power rail and releasing reset before starting I2C. The MMC5983MA uses address `0x30`.

## Behavior

The sketch configures the MMC5983MA with a 100 Hz filter bandwidth, performs a SET operation, enables automatic SET/RESET, and requests 50 Hz continuous measurements. If continuous mode cannot be enabled, it reports the condition and uses single-shot measurements instead.

Every 100 ms, the sketch converts each 18-bit unsigned reading to gauss using `(raw - 131072) / 16384` and prints the X, Y, and Z fields at 115200 baud. Initialization or read failures are reported over Serial.

## Limitations

The output is an uncalibrated magnetic-field vector. The example does not calculate a compass heading, remove hard-iron bias, correct soft-iron scaling, or compensate for board tilt.

The example intentionally supports only the onboard MMC5983MA configurations declared by RC32 and RC52. A board that merely provides a Sensor I2C bus is not considered compatible.
