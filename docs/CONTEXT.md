# RadioCore Hardware Support

This context defines the shared language for Arduino support of RadioCore
series hardware.

## Language

**RadioCore Library**:
The shared Arduino package for the RadioCore hardware family.
_Avoid_: sensor driver bundle, full board support package

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

**Source-level target board**:
A board whose local Arduino variant and example preprocessing path have been
aligned with the source. This does not imply successful compilation or hardware
validation.
_Avoid_: verified board, supported hardware

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
