# E0213A367 E-Paper Display

This example drives the monochrome E0213A367 panel through the RD02E e-paper
driver board on Heltec RC32 and RC52. It renders a changing test page using a
32-row page buffer, performs a full-screen refresh, then rests for 60 seconds.

## Requirements

- A Heltec RC32 or RC52.
- An RD02E e-paper driver board and compatible E0213A367 panel.
- A development version of `heltec-eink-modules` that provides the external
  E0213A367 constructor and RC52 platform support.

## Wiring

RD02E P1 mates pin-for-pin with RC32 P3 or RC52 P1. Confirm the connector pin 1
orientation against the PCB silkscreen before applying power. The table uses
the updated RD02E host signal assignment.

| Signal | RD02E P1 | RC32 P3 | RC52 P1 |
| --- | ---: | ---: | ---: |
| RST/RES | 10 | GPIO16 | P0.28 / D28 |
| BUSY | 24 | GPIO38 | P1.02 / D34 |
| DC | 8 | GPIO17 | P0.30 / D30 |
| EN/VEINK_Ctrl | 26 | GPIO39 | P1.04 / D36 |
| CS | 2 | GPIO6 | P1.13 / D45 |
| SCK/SCL | 1 | GPIO4 | P0.10 / D10 |
| MOSI/SDA | 3 | GPIO5 | P0.09 / D9 |

`VEINK_Ctrl` is active low, BUSY is active high, and RST and CS are active low.
BS1 is tied to ground on RD02E, selecting 4-wire, 8-bit SPI. The interface has
no independent MISO signal. RC52 uses `SPI1`, explicitly remapped to SCK D10 and
MOSI D9; CS remains the separate D45 GPIO.

## Runtime behavior

Each loop asserts RD02E power, waits 100 ms, hard-resets the panel, selects full
refresh mode and draws a new refresh count. The 128-pixel controller width and
32-row page height use a 512-byte image buffer; the visible drawing area is
122 by 250 pixels. After a successful refresh, RD02E remains powered during the
60-second rest period.

BUSY waits time out after 60 seconds. A timeout switches `VEINK_Ctrl` inactive,
prints an error, rests for 60 seconds and retries the complete power/reset
sequence on the next loop.

## Shared-pin restrictions

Run this as a standalone example. Its GPIOs overlap
soil-moisture ADC or power, WS2812 and other display
functions. Do not operate those features at the same time.

RD02E does not isolate MCU logic signals when its switched e-paper supply is
off. After a BUSY timeout, control signals may still create a reverse-current
path; this example does not stop SPI or place every control line in a high-
impedance state.
