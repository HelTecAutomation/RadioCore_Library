# Soil Moisture Read

## Purpose

This example averages an analog soil moisture sensor, maps the raw ADC value between configurable dry and wet calibration points, and prints relative moisture once per second.

## Requirements

- A RadioCore board with a valid soil-moisture ADC configuration
- An analog soil moisture sensor
- A common ground between the sensor supply and the RadioCore board

No additional Arduino library is required.

## Wiring and board differences

| Board | Sensor analog output | Sensor EN |
| --- | --- | --- |
| Heltec RC32 | GPIO6 | GPIO5, active high, 100 ms warm-up |
| Heltec RCC6 | GPIO0 | GPIO2, active high, 100 ms warm-up |
| Heltec RC52 | P1.13 (45), not ADC-capable | P0.09 (9), active high, 100 ms warm-up |

Connect the sensor analog output and enable input to the listed pins, and connect sensor ground to board ground. The configured pins overlap NV3001B signals on these boards; run this as a standalone example without initializing the display.

## Behavior

The sketch enables the configured sensor power control, waits 100 ms, and configures a 12-bit ADC. It averages 16 raw samples per reading, converts the average to a constrained percentage, and prints both values at 115200 baud once per second.

## Calibration

The default calibration maps a raw value of 2560 to 0% and 3980 to 100%:

```cpp
constexpr int32_t kDryValue = 2560;
constexpr int32_t kWetValue = 3980;
```

These values came from the reference sketch and are not universal. Measure and replace both values for the actual sensor and growing medium.

## Limitations

RC52 P1.13 is not ADC-capable. The current RC52 capability macro enables this example at preprocessing time, but the mapping cannot produce valid soil moisture readings until an ADC-capable input is assigned.
