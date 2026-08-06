# NV3001B Display

## Purpose

This example initializes the board-configured 128-by-220 NV3001B color TFT, draws identifying text and color blocks, and updates a counter once per second.

## Requirements

- RadioCore RC32, RCC6, or RC52 board configuration
- The NV3001B-enabled
  [Quency-D/Arduino_GFX](https://github.com/Quency-D/Arduino_GFX) fork

Install the Arduino_GFX fork manually in the Arduino sketchbook `libraries` directory. The Library Manager release does not currently provide the required NV3001B driver.

## Wiring and board differences

| Board | Display transport | SCK | MOSI | CS | DC | RST | EN | BL |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| Heltec RC32 | ESP32 hardware SPI | GPIO17 | GPIO38 | GPIO39 | GPIO16 | GPIO4 | GPIO6, active low | GPIO5, active high |
| Heltec RCC6 | Software SPI | GPIO4 | GPIO15 | GPIO18 | GPIO3 | GPIO0 | GPIO2, active low | GPIO1, active high |
| Heltec RC52 | `SPI1` hardware SPI | P0.30 (30) | P1.02 (34) | P1.04 (36) | P0.28 (28) | P0.10 (10) | P1.13 (45), active low | P0.09 (9), active high |

All three configurations use an 8 MHz SPI clock. The mappings and active levels come from the library board configuration headers.

## Behavior

The sketch configures a 128-by-220 panel with rotation 3. It enables panel power while keeping the backlight off, initializes the selected SPI transport, draws the initial frame, and only then turns on the backlight. The display shows the board name, panel model, color blocks, and a counter updated every second.

Initialization status is also printed at 115200 baud.

## Limitations

The example trusts the compile-time board configuration and does not read or validate the NV3001B controller ID. It stops updating if display initialization fails.
