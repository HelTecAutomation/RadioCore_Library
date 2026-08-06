# BH1750 Read

## Purpose

This example reads ambient light from an external BH1750 sensor on the
board-configured Sensor I2C bus and prints the result once per second.

## Requirements

- RadioCore RC32, RCC6, or RC52 board configuration
- [claws/BH1750](https://github.com/claws/BH1750) version 1.3.0 or newer

The BH1750 dependency is declared in `library.properties` and can be installed
automatically by dependency-aware Arduino tools.

## Wiring and board differences

Connect the BH1750 module as follows:

| BH1750 | RadioCore board |
| --- | --- |
| VCC | Board sensor-power or 3.3 V rail |
| GND | GND |
| SDA | Sensor I2C SDA |
| SCL | Sensor I2C SCL |
| ADDR | GND/open for `0x23`, or 3.3 V for `0x5C` |

| Board | SDA | SCL | Sensor power control |
| --- | ---: | ---: | --- |
| Heltec RC32 | GPIO21 | GPIO18 | GPIO46, active high, 100 ms warm-up |
| Heltec RCC6 | GPIO2 | GPIO1 | Not configured; power the sensor externally |
| Heltec RC52 | P1.11 (43) | P0.02 (2) | P0.12 (12), active high, 100 ms warm-up |

The mappings come from the library board configuration headers. RC32 and RC52
enable their Sensor power rail before starting I2C. RCC6 does not have an
authoritative sensor-power control in the current configuration.

## Behavior

The sketch starts Sensor I2C at 100 kHz, checks for an acknowledgement at
`0x23` and `0x5C`, and passes the detected address and configured bus instance
to the BH1750 driver. It uses continuous high-resolution mode and prints lux at
115200 baud once per second when a measurement is ready.

If neither address responds, or if the driver cannot initialize the responding
device, the sketch prints a diagnostic message and does not attempt readings.

## Limitations

The example supports the configured ESP32/WiFi Kit Series and Heltec nRF52 I2C
APIs. A board using a different I2C API needs an additional architecture path.
