# RadioCore Library

RadioCore Library is the shared Arduino library for RadioCore series hardware.

## Current status

Version 0.0.1 provides compile-time hardware facts for RadioCore examples. The
BH1750 example has source-level target configuration for Heltec RC52, RC32, and
RCC6. This does not claim that the example has been compiled or tested on
hardware.

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

## Adding a board configuration

Add a hardware-facts-only header under `src/boards/`, then select it from
`RadioCoreBoardConfig.h`. A board using the existing ESP32
`begin(SDA, SCL, frequency)` API or the current Heltec nRF52 `setPins()`,
`begin()`, and `setClock()` API can reuse `BH1750_Read` without changing the
example. A board-controlled Sensor power rail can additionally declare its
control pin, active level, and warmup time. A board with a different I2C API,
dedicated bus object, or power sequence may require a new architecture path in
the example.

Unknown boards can still include `RadioCore.h`; they receive
`RADIOCORE_HAS_SENSOR_I2C=0`. The BH1750 example performs its own capability
check and reports an unsupported configuration during preprocessing.

## License

RadioCore Library is released under the MIT License. See [LICENSE](LICENSE).
