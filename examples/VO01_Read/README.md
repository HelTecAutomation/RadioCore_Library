# VO01 Read

## Purpose

This example samples an analog VO01 gas sensor, restores the sensor voltage before the board's 2:1 resistor divider, estimates concentration, and reports a 30-second concentration trend.

## Requirements

- RadioCore RC32 or RC52 board configuration
- A VO01 sensor connected through the expected 2:1 divider
- A common ground between the sensor supply and the RadioCore board

No additional Arduino library is required.

## Wiring and board differences

| Board | Divided analog signal | Sensor power control |
| --- | --- | --- |
| Heltec RC32 | GPIO2 | GPIO46, active high, 100 ms warm-up |

Connect the sensor analog output through the hardware's 2:1 divider to the ADC pin shown above. Do not exceed the ADC input range. 

## Behavior

The sketch enables board-controlled sensor power when available, configures a 12-bit ADC, and averages 16 samples for each reading. ESP32-based boards use `analogReadMilliVolts()` with maximum attenuation; RC52 uses the 3.0 V internal reference and converts the averaged raw ADC value to millivolts.

It prints restored sensor voltage and estimated concentration at 115200 baud once per second. Every 30 seconds it compares the current estimate with the previous checkpoint and reports increased, decreased, or unchanged.

## Calibration

The default conversion is:

```text
concentration_ppm = max(0, (restored_voltage_V - 2.25) * 50)
```

The 2.25 V zero point is only a default. Measure and replace `kZeroPointVoltage` for each sensor before relying on the concentration value.
The result is an example estimate, not a certified gas measurement.
