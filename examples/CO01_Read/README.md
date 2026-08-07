# CO01 Read

## Purpose

This example runs the required one-time CO01 startup sequence, samples the analog output, restores the sensor voltage before the board's 2:1 resistor divider, estimates concentration, and reports a 30-second trend.

## Requirements

- RadioCore RC32 board configuration
- A CO01 sensor connected through the expected 2:1 divider
- A common ground between the sensor supply and the RadioCore board

No additional Arduino library is required.

## Wiring and board differences

| Board | Divided analog signal | CO01 control | Sensor power control |
| --- | --- | --- | --- |
| Heltec RC32 | GPIO2 | GPIO21 | GPIO46, active high, 100 ms warm-up |

Connect the sensor analog output through the hardware's 2:1 divider to the ADC pin shown above, and connect the sensor control input to the listed control pin. Do not exceed the ADC input range.

## Behavior

At startup, the sketch enables board-controlled sensor power when available, drives the CO01 control pin high for 60 seconds, then drives it low for 90 seconds. This blocking warm-up cycle runs once, prints each phase at 115200 baud, and leaves the control pin low.

After warm-up, the sketch configures a 12-bit ADC and averages 16 samples per reading. It prints restored sensor voltage and estimated concentration once per second. Every 30 seconds it compares the current estimate with the previous checkpoint and reports increased, decreased, or unchanged.

## Calibration

The default conversion is:

```text
concentration_ppm = max(0, (restored_voltage_V - 1.15) * 150)
```

The 1.15 V zero point is only a default. Measure and replace `kZeroPointVoltage` for each sensor before relying on the concentration value.
The result is an example estimate, not a certified gas measurement.
