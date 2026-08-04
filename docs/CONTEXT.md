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
