# RadioCore_Kit

RadioCore_Kit is the shared Arduino library for RadioCore series hardware.
It provides compile-time board configuration used by portable, board-adaptive
examples.

## Installation

Open Library Manager in the Arduino IDE, search for `RadioCore_Kit`, and select
**Install**. Arduino CLI users can install the library with:

```sh
arduino-cli lib install RadioCore_Kit
```

For development versions that have not been released, clone or copy this
repository into the `libraries` directory of your Arduino sketchbook, then
restart the Arduino IDE if it is already running.

Sketches can include the library with:

```cpp
#include <RadioCore_Kit.h>
```

## Dependencies

The library declares these Arduino dependencies:

- `Adafruit BME280 Library`
- `BH1750` version 1.3.0 or newer from
  [claws/BH1750](https://github.com/claws/BH1750)
- `SparkFun MMC5983MA Magnetometer Arduino Library` version 1.1.5 or newer
  for the onboard MMC5983MA magnetometer
- `ICM42670P` version 1.0.8 or newer from TDK InvenSense as the compatible
  driver used by the onboard ICM42607P example
- `Adafruit NeoPixel`
- `ESP32Servo` for ESP32-based RadioCore boards
- `NonBlockingRTTTL` version 1.4.0 or newer for non-blocking buzzer melodies

Dependency-aware Arduino tools can install these libraries automatically.
RadioCore_Kit does not copy, wrap, or re-export them.

The Heltec nRF52 board package supplies the Arduino official `Servo` library
used by `Servo_Control` on RC52; no separate Servo library installation is
required.

`NV3001B_Display` additionally requires the NV3001B-enabled
[Quency-D/Arduino_GFX](https://github.com/Quency-D/Arduino_GFX) fork. Install
that fork manually in the Arduino sketchbook `libraries` directory. It is not
listed in `library.properties` because NV3001B support has not been merged into
the Arduino Library Manager version of `GFX Library for Arduino`.

`DEPG1020BNS770F1_Display` and `E0213A367_Display` require the development
version of `heltec-eink-modules` containing the external RD02E constructors and
RC52 platform support. Install that version manually until the changes are
available in a published Library Manager release.

## Examples

Open the examples from `File > Examples > RadioCore_Kit` in the Arduino
IDE. Board configuration macros select the supported pins and peripherals at
compile time.

| Example | Purpose | Documentation |
| --- | --- | --- |
| `BH1750_Read` | Read ambient light over Sensor I2C | [Guide](examples/BH1750_Read/README.md) |
| `MMC5983MA_Read` | Read the onboard three-axis magnetic field in gauss on RC32 and RC52 | [Guide](examples/MMC5983MA_Read/README.md) |
| `ICM42607P_Read` | Read onboard acceleration in g and angular rate in degrees per second on RC32 and RC52 | [Guide](examples/ICM42607P_Read/README.md) |
| `CH01_Read` | Estimate CH01 gas concentration and trend | [Guide](examples/CH01_Read/README.md) |
| `CO01_Read` | Run the CO01 warm-up cycle and estimate concentration | [Guide](examples/CO01_Read/README.md) |
| `VO01_Read` | Estimate VO01 gas concentration and trend | [Guide](examples/VO01_Read/README.md) |
| `Soil_Moisture_Read` | Read and calibrate an analog soil moisture sensor | [Guide](examples/Soil_Moisture_Read/README.md) |
| `NV3001B_Display` | Exercise the 128-by-220 color TFT | [Guide](examples/NV3001B_Display/README.md) |
| `DEPG1020BNS770F1_Display` | Startup black/white test, full baseline, and partial-window updates on the 960-by-640 panel (RC32/RC52) | [Guide](examples/DEPG1020BNS770F1_Display/README.md) |
| `E0213A367_Display` | Refresh the 122-by-250 drawable E0213A367 e-paper panel on RC32 or RC52 | [Guide](examples/E0213A367_Display/README.md) |
| `Rotary_Encoder` | Report rotary direction, signed count, and user-button presses on RC32 and RC52 | [Sketch](examples/Rotary_Encoder/Rotary_Encoder.ino) |
| `WS2812` | Run a three-color breathing animation | [Sketch](examples/WS2812/WS2812.ino) |
| `Buzzer_Control` | Play and repeat an RTTTL melody through the board-designated buzzer output | [Sketch](examples/Buzzer_Control/Buzzer_Control.ino) |
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
- Onboard MMC5983MA and ICM42607P capability flags plus any shared sensor-reset
  pin and release level required by their examples.
- ADC pins and optional power controls for `CH01_Read`, `VO01_Read`,
  `CO01_Read`, and `Soil_Moisture_Read`; CO01 also requires its control pin.
- NV3001B pins, active levels, SPI frequency, and one supported display
  transport for `NV3001B_Display`.
- RD02E driver-board pins and active levels for the shared paged
  DEPG1020BNS770F1 and E0213A367 examples. Document the connector mapping and
  shared-pin conflicts.
- A rotary encoder I2C address and phase masks for `Rotary_Encoder`, plus an
  independently declared user-button pin, active level, and input mode. The
  current RC32 and RC52 mappings share the configured Sensor I2C bus and power
  rail.
- A data pin for `WS2812`, a buzzer output pin and any shared peripheral power
  control required by `Buzzer_Control`, two motor-driver inputs for
  `Motor_Control`, a relay output and active level for `Relay_Control`, and two
  PWM pins plus driver enable facts for `Servo_Control`.

The shared examples currently support the ESP32/WiFi Kit Series and Heltec
nRF52 peripheral APIs used by RC32, RCC6, and RC52. A board with a different
I2C, ADC, SPI, or servo API may require a new architecture path in the relevant
example.

Unknown boards can still include `RadioCore_Kit.h`. All example capability macros
default to disabled, and an unsupported example reports its missing capability
during preprocessing.

## License

RadioCore_Kit is released under the MIT License. See [LICENSE](LICENSE).
