# RadioCore Library

RadioCore Library is the shared Arduino library for RadioCore series hardware.

## Current status

Version 0.0.1 provides compile-time hardware facts for RadioCore examples. The
BH1750, Soil_Moisture_Read, NV3001B, WS2812, and Motor_Control examples have
source-level target configuration for Heltec RC52, RC32, and RCC6. The
Relay_Control example has the same source-level target coverage. This does not
claim that the examples have been compiled or tested on hardware.

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
- `Adafruit NeoPixel`

Dependency-aware Arduino tools can install them automatically. RadioCore
Library does not copy, wrap, or re-export these libraries.

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

## WS2812 example

Open `File > Examples > RadioCore Library > WS2812` in the Arduino IDE. The
example drives a 30-pixel WS2812 strip using `NEO_GRB + NEO_KHZ800`. It starts
automatically at power-up and cycles pale yellow, pale blue, and white through
a six-second breathing period for each color. The global brightness is limited
to 128, or 50% of the library's full scale.

Change `LED_COUNT` near the top of `WS2812.ino` when using a strip with a
different number of pixels. The source-level DIN mappings are:

| Board | WS2812 DIN |
| --- | --- |
| Heltec RC32 | GPIO5 |
| Heltec RCC6 | GPIO2 |
| Heltec RC52 | P0.09 (9) |

Power the strip from a supply sized for its pixel count and brightness, and
connect the strip ground to the RadioCore board ground. Do not power a
30-pixel strip directly from a GPIO pin.

This is a standalone example. Its DIN pins overlap other configured functions:
RC32 GPIO5 is also the NV3001B backlight and soil-moisture power control, RCC6
GPIO2 is also the NV3001B enable and soil-moisture power control, and RC52
P0.09 is also the NV3001B backlight and soil-moisture power control. The
example does not initialize those peripherals.

These mappings are defined by the library's board configuration headers.
Compilation and hardware behavior have not been verified as part of this
change.

## Motor control example

Open `File > Examples > RadioCore Library > Motor_Control` in the Arduino IDE.
The example controls two digital inputs on an external motor driver with a
non-blocking state machine. At startup both inputs remain low for 10 seconds.
The first run drives IN1 high and IN2 low for 3 seconds, followed by 17 seconds
with both inputs low. Every later run drives IN1 low and IN2 high for 3 seconds,
then returns to the same 17-second stopped interval. The later runs remain in
the IN2 direction; they do not alternate direction.

Connect the external motor driver inputs as follows:

| Board | Motor driver IN1 | Motor driver IN2 |
| --- | ---: | ---: |
| Heltec RC32 | GPIO41 | GPIO42 |
| Heltec RCC6 | GPIO3 | GPIO4 |
| Heltec RC52 | P1.01 (33) | P0.20 (20) |

Connect the motor driver logic ground to the RadioCore board ground. Do not
connect a motor directly to either GPIO; use a suitable motor driver and motor
power supply.

This is a standalone example and does not initialize other peripherals. On
RCC6, GPIO3 and GPIO4 overlap the configured NV3001B display DC and SCK signals,
respectively, so the display cannot be operated at the same time with these
motor mappings.

These mappings are defined by the library's board configuration headers.
Compilation and hardware behavior have not been verified as part of this
change.

## Relay control example

Open `File > Examples > RadioCore Library > Relay_Control` in the Arduino IDE.
The example starts with the relay released for three seconds, then alternates
between activated and released every three seconds. It uses a blocking delay
and prints `Relay activated` or `Relay released` at 115200 baud after each
state change. Override `RELAY_TOGGLE_INTERVAL_MS` to use a different interval.

Connect the logic input of an active-high relay module as follows:

| Board | Relay module input | Active level |
| --- | ---: | --- |
| Heltec RC32 | GPIO2 | High |
| Heltec RCC6 | GPIO1 | High |
| Heltec RC52 | P1.15 (47) | High |

Connect the relay module logic ground to the RadioCore board ground. Do not
drive a relay coil directly from a GPIO; use a suitable relay module or driver
and an appropriate supply for the relay and its load.

This is a standalone example and does not initialize other peripherals. RC32
GPIO2 overlaps the configured analog input used by the gas sensor examples.
RCC6 GPIO1 overlaps Sensor I2C SCL and the configured NV3001B backlight, so
those functions cannot be operated at the same time with this relay mapping.

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
WS2812-capable boards declare the strip data pin used by the shared example.
Motor-control-capable boards declare the two digital inputs used by the shared
Motor_Control example.
Relay-control-capable boards declare the digital output and active level used
by the shared Relay_Control example.

Unknown boards can still include `RadioCore.h`; they receive
`RADIOCORE_HAS_SENSOR_I2C=0`, `RADIOCORE_HAS_SOIL_MOISTURE_ADC=0`, and
`RADIOCORE_HAS_WS2812=0`, `RADIOCORE_HAS_MOTOR_CONTROL=0`, and
`RADIOCORE_HAS_RELAY_CONTROL=0`. The BH1750, Soil_Moisture_Read, WS2812,
Motor_Control, and Relay_Control examples perform their own capability checks
and report unsupported configurations during preprocessing.

## License

RadioCore Library is released under the MIT License. See [LICENSE](LICENSE).
