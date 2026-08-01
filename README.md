# RadioCore Library

RadioCore Library is the shared Arduino library for RadioCore series hardware.

## Current status

Version 0.0.1 establishes the standard Arduino library structure. It provides
the public include entry point `RadioCore.h`, but does not expose any classes,
functions, or board-specific initialization yet.

## Installation

Clone or copy this repository into the `libraries` directory of your Arduino
sketchbook, then restart the Arduino IDE if it is already running.

Sketches can include the library with:

```cpp
#include <RadioCore.h>
```

## Dependency

The library declares `Adafruit BME280 Library` as an Arduino dependency.
Dependency-aware Arduino tools can install it automatically. Code using the
BME280 should include and call the Adafruit library directly; RadioCore Library
does not wrap or re-export the Adafruit BME280 API.

## License

RadioCore Library is released under the MIT License. See [LICENSE](LICENSE).
