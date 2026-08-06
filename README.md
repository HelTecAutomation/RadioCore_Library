# RadioCore Library

RadioCore Library is the shared Arduino library for RadioCore series hardware.
It provides compile-time board configuration used by portable, board-adaptive
examples.

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
- `ESP32Servo` for ESP32-based RadioCore boards

Dependency-aware Arduino tools can install these libraries automatically.
RadioCore Library does not copy, wrap, or re-export them.

The Heltec nRF52 board package supplies the Arduino official `Servo` library
used by `Servo_Control` on RC52; no separate Servo library installation is
required.

`NV3001B_Display` additionally requires the NV3001B-enabled
[Quency-D/Arduino_GFX](https://github.com/Quency-D/Arduino_GFX) fork. Install
that fork manually in the Arduino sketchbook `libraries` directory. It is not
listed in `library.properties` because NV3001B support has not been merged into
the Arduino Library Manager version of `GFX Library for Arduino`.

## Examples

Open the examples from `File > Examples > RadioCore Library` in the Arduino
IDE. Board configuration macros select the supported pins and peripherals at
compile time.

| Example | Purpose | Documentation |
| --- | --- | --- |
| `BH1750_Read` | Read ambient light over Sensor I2C | [Guide](examples/BH1750_Read/README.md) |
| `CH01_Read` | Estimate CH01 gas concentration and trend | [Guide](examples/CH01_Read/README.md) |
| `CO01_Read` | Run the CO01 warm-up cycle and estimate concentration | [Guide](examples/CO01_Read/README.md) |
| `VO01_Read` | Estimate VO01 gas concentration and trend | [Guide](examples/VO01_Read/README.md) |
| `Soil_Moisture_Read` | Read and calibrate an analog soil moisture sensor | [Guide](examples/Soil_Moisture_Read/README.md) |
| `NV3001B_Display` | Exercise the 128-by-220 color TFT | [Guide](examples/NV3001B_Display/README.md) |
| `WS2812` | Run a three-color breathing animation | [Sketch](examples/WS2812/WS2812.ino) |
| `Motor_Control` | Drive two direction inputs on an external motor driver | [Sketch](examples/Motor_Control/Motor_Control.ino) |
| `Relay_Control` | Toggle an active-high relay module | [Sketch](examples/Relay_Control/Relay_Control.ino) |
| `Servo_Control` | Control both channels of an RS-SV01 servo driver | [Sketch](examples/Servo_Control/Servo_Control.ino) |

Guided examples include their dependencies, wiring, board differences, runtime
behavior, and applicable calibration or limitation notes. Basic examples keep
their essential connection and safety notes in the sketch header.

## Adding a board configuration

Add a hardware-facts-only header under `src/boards/`, then select it from
`src/RadioCoreBoardConfig.h`. Keep peripheral initialization and example
behavior out of the board header.

A new board declares only the capabilities it supports:

- Sensor I2C instance, pins, frequency, and optional sensor-power control for
  `BH1750_Read`.
- ADC pins and optional power controls for `CH01_Read`, `VO01_Read`,
  `CO01_Read`, and `Soil_Moisture_Read`; CO01 also requires its control pin.
- NV3001B pins, active levels, SPI frequency, and one supported display
  transport for `NV3001B_Display`.
- A data pin for `WS2812`, two motor-driver inputs for `Motor_Control`, a relay
  output and active level for `Relay_Control`, and two PWM pins plus driver
  enable facts for `Servo_Control`.

The shared examples currently support the ESP32/WiFi Kit Series and Heltec
nRF52 peripheral APIs used by RC32, RCC6, and RC52. A board with a different
I2C, ADC, SPI, or servo API may require a new architecture path in the relevant
example.

Unknown boards can still include `RadioCore.h`. All example capability macros
default to disabled, and an unsupported example reports its missing capability
during preprocessing.

## License

RadioCore Library is released under the MIT License. See [LICENSE](LICENSE).
