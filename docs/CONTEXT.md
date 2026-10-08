# RadioCore Hardware Support

This context defines the shared language for Arduino support of RadioCore
series hardware.

## Language

**RadioCore_Kit**:
The shared Arduino package for the RadioCore hardware family.
_Avoid_: RadioCore Library, sensor driver bundle, full board support package

**Sensor I2C**:
The board-designated I2C bus used by RadioCore onboard and external sensors.
It may map to different `TwoWire` instances or pins on different boards.
_Avoid_: default Wire, I2C0

**Sensor power rail**:
The board-controlled power domain that supplies external sensors and may be
shared with other peripherals.
_Avoid_: sensor VCC pin, sensor enable pin, Vext

**MMC5983MA magnetometer**:
The board-integrated three-axis magnetic-field sensor on Sensor I2C in RC32
and RC52.
_Avoid_: MMC5983, compass module, external magnetometer

**ICM42607P motion sensor**:
The board-integrated inertial sensor on Sensor I2C in RC32 and RC52; its
compatible Arduino driver dependency is named ICM42670P.
_Avoid_: ICM42670P sensor, external IMU

**Board configuration**:
Compile-time hardware facts for one RadioCore board, without peripheral
initialization or example behavior.
_Avoid_: board driver, initialization layer

**NV3001B display**:
The 128-by-220 color TFT panel configured by RadioCore display examples.
_Avoid_: OLED, generic screen

**Display transport**:
The board-selected SPI mechanism used to send commands and pixels to a
configured display device, including the NV3001B display or an e-paper driver
board.
_Avoid_: default SPI, display bus

**DEPG1020BNS770F1 e-paper panel**:
The 960-by-640 monochrome e-paper panel driven by an SSD1677 controller.
_Avoid_: generic 10.2-inch display, TFT, grayscale panel

**E0213A367 e-paper panel**:
The 128-by-250 monochrome e-paper panel with a 122-by-250 drawable area,
connected through the RD02E e-paper driver board in RadioCore examples.
_Avoid_: generic 2.13-inch display, Wireless Paper display

**DEPG0290BNS800 e-paper panel**:
The 128-by-296 monochrome e-paper panel identified by its FPC-7519 rev.b
connector marking and supported by the matching heltec-eink-modules driver.
_Avoid_: generic 2.9-inch display, DEPG0290BNS75A, Vision Master display

**RD02E e-paper driver board**:
The external board that supplies switched power and high-voltage drive circuits
for compatible e-paper panels, including DEPG1020BNS770F1, E0213A367, and
DEPG0290BNS800, and exposes their controller logic interface.
_Avoid_: bare panel, passive adapter, TFT connector

**Full-screen e-paper refresh**:
A monochrome update that refreshes the complete e-paper image in one operation.
_Avoid_: partial refresh, fast refresh, grayscale refresh

**Partial-window e-paper refresh**:
A monochrome update that uses the panel's partial-refresh waveform to update
an explicitly selected region while preserving the surrounding displayed image.
_Avoid_: full-screen refresh, automatic change detection, grayscale refresh

**Fast-window e-paper update**:
A differential fast refresh whose image-RAM writes are restricted to a selected
window while the surrounding displayed image is preserved; it does not imply
that the controller drives only the selected gate region.
_Avoid_: proven local gate scan, full-screen refresh, grayscale refresh

**E-paper reference image**:
Controller-side image data that provides a baseline for subsequent partial
refreshes, distinct from the MCU drawing buffer and the physically visible image.
_Avoid_: MCU framebuffer, visible image

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

**Buzzer output**:
The board-designated PWM output connected to a piezo buzzer or buzzer module.
_Avoid_: tone pin, speaker output

**Motor control input**:
One of a pair of digital direction signals connected to an external motor
driver; together the pair selects a stopped state or a drive direction.
_Avoid_: PWM output, motor power pin

**Relay control output**:
The board-designated digital signal connected to the logic input of an
external relay module or driver.
_Avoid_: relay coil output, load power output

**Rotary encoder input**:
The board-designated two-channel user control whose rotation produces previous
or next steps.
_Avoid_: rotary button, generic GPIO input

**User button input**:
The board-designated momentary digital input reported independently from
rotary encoder movement.
_Avoid_: rotary encoder press, enter key

**Servo control interface**:
The board-designated pair of servo PWM signals and enable signal used to
connect an external dual-servo driver such as the RS-SV01.
_Avoid_: servo motor, generic PWM module
