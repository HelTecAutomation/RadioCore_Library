# RadioCore Library

RadioCore Library is the shared Arduino library for RadioCore series hardware.

## Current status

Version 0.0.1 provides compile-time hardware facts for RadioCore examples. The
BH1750, Soil_Moisture_Read, and NV3001B examples have source-level target
configuration for Heltec RC52, RC32, and RCC6. This does not claim that the
examples have been compiled or tested on hardware.

## Installation

Clone or copy this repository into the `libraries` directory of your Arduino
sketchbook, then restart the Arduino IDE if it is already running.

Sketches can include the library with:

```cpp
#include <RadioCore.h>
```

## Dependencies

The library declares these Arduino dependencies:

- `Adafruit BME280 Library`
- `BH1750` version 1.3.0 or newer from
  [claws/BH1750](https://github.com/claws/BH1750)

Dependency-aware Arduino tools can install them automatically. RadioCore
Library does not copy, wrap, or re-export either sensor driver.

The `NV3001B_Display` example additionally requires the NV3001B-enabled
[Quency-D/Arduino_GFX](https://github.com/Quency-D/Arduino_GFX) fork. Install
that fork manually in the Arduino sketchbook `libraries` directory. It is not
listed in `library.properties` because NV3001B support has not been merged into
the Arduino Library Manager version of `GFX Library for Arduino`.

## NV3001B display example

Open `File > Examples > RadioCore Library > NV3001B_Display` in the Arduino
IDE. One sketch selects the board-specific display transport and pins at
compile time, initializes the NV3001B in landscape rotation 3, then displays
the board name, panel model, color blocks, and a counter updated once per
second.

The source-level target mappings are:

| Board | Transport | SCK | MOSI/SDA | CS | DC | RST | EN | BL |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| Heltec RC32 | ESP32 hardware SPI, 8 MHz | 17 | 38 | 39 | 16 | 4 | 6, active low | 5, active high |
| Heltec RCC6 | Software SPI | 4 | 15 | 18 | 3 | 0 | 2, active low | 1, active high |
| Heltec RC52 | `SPI1`, 8 MHz | P0.30 (30) | P1.02 (34) | P1.04 (36) | P0.28 (28) | P0.10 (10) | P1.13 (45), active low | P0.09 (9), active high |

The example keeps the backlight off during panel initialization and enables it
only after the initial frame has been drawn. It does not read or validate the
NV3001B controller ID.

## BH1750 read example

Open `File > Examples > RadioCore Library > BH1750_Read` in the Arduino IDE.
The example initializes the board's configured Sensor I2C bus at 100 kHz,
checks for an I2C acknowledgement at addresses `0x23` and `0x5C`, and only then
passes the responding address and bus instance to the claws BH1750 driver. It
prints a lux reading once per second in continuous high-resolution mode.

Connect an external BH1750 module as follows:

| BH1750 | RadioCore board |
| --- | --- |
| VCC | Board sensor-power/3.3V rail |
| GND | GND |
| SDA | Sensor I2C SDA |
| SCL | Sensor I2C SCL |
| ADDR | GND/open for `0x23`, or 3.3V for `0x5C` |

The source-level target bus and power mappings are:

| Board | SDA | SCL | Sensor power control |
| --- | ---: | ---: | --- |
| Heltec RC32 | 21 | 18 | GPIO46, active high, 100 ms warmup |
| Heltec RCC6 | 2 | 1 | Not configured; power the sensor externally |
| Heltec RC52 | P1.11 (43) | P0.02 (2) | P0.12, active high, 100 ms warmup |

For RC32 and RC52, the example enables the board's Sensor power rail and waits
for it to stabilize before initializing Sensor I2C. RCC6 intentionally retains
only an I2C configuration placeholder until an authoritative sensor-power
control is available.

These mappings are defined by the library's board configuration headers.
Compilation and hardware behavior have not been verified as part of this
change.

## Soil moisture read example

Open `File > Examples > RadioCore Library > Soil_Moisture_Read` in the Arduino
IDE. The example enables the configured sensor power control, waits 100 ms,
then samples the configured 12-bit ADC input 16 times per reading. Once per
second it prints the averaged raw ADC value and a moisture percentage.

Connect an analog soil moisture sensor as follows:

| Board | Sensor analog output | Sensor EN |
| --- | --- | --- |
| Heltec RC32 | GPIO6 | GPIO5, active high |
| Heltec RCC6 | GPIO0 | GPIO2, active high |
| Heltec RC52 | P1.13 (45), not ADC-capable | P0.09 (9), active high |

> **RC52 limitation:** P1.13 cannot be used as an ADC input. The current RC52
> pin mapping therefore cannot produce soil moisture readings; select an
> ADC-capable RC52 pin before using this example on that board.

The example maps a raw value of 2560 to 0% and 3980 to 100%, then constrains the
result to that range. These values are defaults from the reference sensor
sketch, not universal calibration data. Measure and replace both calibration
points for the actual sensor and growing medium before relying on the reported
percentage.

The configured soil moisture pins overlap signals used by the NV3001B display
on these boards. `Soil_Moisture_Read` is a standalone example and does not
initialize or operate the display.

These mappings are defined by the library's board configuration headers.
Compilation and hardware behavior have not been verified as part of this
change.

## Adding a board configuration

Add a hardware-facts-only header under `src/boards/`, then select it from
`RadioCoreBoardConfig.h`. A board using the existing ESP32
`begin(SDA, SCL, frequency)` API or the current Heltec nRF52 `setPins()`,
`begin()`, and `setClock()` API can reuse `BH1750_Read` without changing the
example. A board-controlled Sensor power rail can additionally declare its
control pin, active level, and warmup time. A board with a different I2C API,
dedicated bus object, or power sequence may require a new architecture path in
the example. A soil-moisture-capable board declares its ADC input and, when
available, its power-control pin, active level, and warmup time.
NV3001B-capable boards additionally declare display pins, active levels, SPI
frequency, and one of the supported display transports.

Unknown boards can still include `RadioCore.h`; they receive
`RADIOCORE_HAS_SENSOR_I2C=0` and `RADIOCORE_HAS_SOIL_MOISTURE_ADC=0`. The
BH1750 and Soil_Moisture_Read examples perform their own capability checks and
report unsupported configurations during preprocessing.

## License

RadioCore Library is released under the MIT License. See [LICENSE](LICENSE).
