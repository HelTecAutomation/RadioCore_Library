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

**CH01 gas sensor**:
The analog-output gas sensor measured by RadioCore examples.
_Avoid_: gas channel, CH01 input

**Restored CH01 voltage**:
The CH01 output voltage before the external resistor divider, recovered from
the voltage measured at the ADC pin.
_Avoid_: raw ADC voltage, GPIO2 voltage

**CH01 zero-point voltage**:
The restored CH01 voltage that represents a concentration of zero ppm.
_Avoid_: startup voltage, first reading

**Concentration trend**:
The direction of change between two consecutive CH01 concentration checkpoints.
_Avoid_: alarm state, concentration status
