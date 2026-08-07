# RadioCore Hardware Support

This context defines the shared language for Arduino support of RadioCore
series hardware.

## Language

**RadioCore_Kit**:
The shared Arduino package for the RadioCore hardware family.
_Avoid_: RadioCore Library, sensor driver bundle, full board support package

**Sensor I2C**:
The board-designated I2C bus used by RadioCore examples for external sensors.
It may map to different `TwoWire` instances or pins on different boards.
_Avoid_: default Wire, I2C0

**Sensor power rail**:
The board-controlled power domain that supplies external sensors and may be
shared with other peripherals.
_Avoid_: sensor VCC pin, sensor enable pin, Vext

**Board configuration**:
Compile-time hardware facts for one RadioCore board, without peripheral
initialization or example behavior.
_Avoid_: board driver, initialization layer

**NV3001B display**:
The 128-by-220 color TFT panel configured by RadioCore display examples.
_Avoid_: OLED, generic screen

**Display transport**:
The board-selected SPI mechanism used to send commands and pixels to the
NV3001B display.
_Avoid_: default SPI, display bus

**Analog gas sensor**:
A CH01, VO01, or CO01 device that represents gas concentration as an analog output
voltage.
_Avoid_: gas channel, gas input

**Restored sensor voltage**:
The analog gas sensor output voltage before the external resistor divider,
recovered from the voltage measured at the ADC pin.
_Avoid_: raw ADC voltage, GPIO2 voltage

**Zero-point voltage**:
The restored sensor voltage that represents a concentration of zero ppm for an
individual analog gas sensor.
_Avoid_: startup voltage, first reading

**Concentration trend**:
The direction of change between two consecutive analog gas sensor concentration
checkpoints.
_Avoid_: alarm state, concentration status

**Warm-up cycle**:
The one-time CO01 startup sequence that drives its control line high and then
low before concentration measurements begin.
_Avoid_: conditioning cycle, startup delay

**Soil moisture sensor**:
An analog probe whose calibrated output represents relative moisture in a
specific growing medium.
_Avoid_: soil humidity sensor, soil sensor

**Dry calibration point**:
The soil moisture sensor reading selected to represent 0% moisture.
_Avoid_: dry value, minimum moisture

**Wet calibration point**:
The soil moisture sensor reading selected to represent 100% moisture.
_Avoid_: wet value, maximum moisture

**WS2812 LED strip**:
An external chain of individually addressable RGB pixels controlled by a
single data signal.
_Avoid_: generic RGB LED, onboard LED

**Motor control input**:
One of a pair of digital direction signals connected to an external motor
driver; together the pair selects a stopped state or a drive direction.
_Avoid_: PWM output, motor power pin

**Relay control output**:
The board-designated digital signal connected to the logic input of an
external relay module or driver.
_Avoid_: relay coil output, load power output

**Servo control interface**:
The board-designated pair of servo PWM signals and enable signal used to
connect an external dual-servo driver such as the RS-SV01.
_Avoid_: servo motor, generic PWM module
