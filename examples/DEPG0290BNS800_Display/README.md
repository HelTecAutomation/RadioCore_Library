# DEPG0290BNS800 E-Paper Display

This example drives the monochrome DEPG0290BNS800 panel through an RD02E
e-paper driver board on Heltec RC32 and RC52. It performs cold-start coverage
tests, builds a full-screen image baseline, then repeatedly updates a selected
window with the driver's differential fast-refresh mode.

## Requirements

- A Heltec RC32 or RC52.
- An RD02E e-paper driver board and an electrically compatible
  DEPG0290BNS800 panel marked FPC-7519 rev.b.

## Library dependency

This example requires `heltec-eink-modules` from the Quency-D development fork
at commit `7f5f0dafe3bce0a39350901ae3cc8c745463045f`. The public Library Manager
`4.6.0` package does not contain the required DEPG0290BNS800 external-driver
constructor or RC52 platform support.

From the Arduino sketchbook `libraries` directory, install the verified
revision with:

```powershell
git clone https://github.com/Quency-D/heltec-eink-modules.git
git -C heltec-eink-modules checkout 7f5f0dafe3bce0a39350901ae3cc8c745463045f
```

`GFX_Root` is included inside `heltec-eink-modules`; do not install it
separately. The Heltec Arduino core that provides the RC32 or RC52 board target
is a board environment requirement rather than an Arduino library dependency.
This example does not claim a minimum core version.

The host-side connector mapping is shared with the other RD02E examples, but
that does not by itself prove panel-side compatibility. Before first power-up,
confirm the FPC pinout, panel voltage and high-voltage waveform requirements
against the hardware supplied for the panel. Do not connect the panel FPC
directly to RC32 or RC52.

## Wiring

RD02E P1 mates pin-for-pin with RC32 P3 or RC52 P1. Confirm connector pin 1
against the PCB silkscreen before applying power.

| Signal | RD02E P1 | RC32 P3 | RC52 P1 |
| --- | ---: | ---: | ---: |
| SCK/SCL | 1 | GPIO4 | P0.10 / D10 |
| CS | 2 | GPIO6 | P1.13 / D45 |
| MOSI/SDA | 3 | GPIO5 | P0.09 / D9 |
| DC | 8 | GPIO17 | P0.30 / D30 |
| RST/RES | 10 | GPIO16 | P0.28 / D28 |
| BUSY | 24 | GPIO38 | P1.02 / D34 |
| EN/VEINK_Ctrl | 26 | GPIO39 | P1.04 / D36 |

`VEINK_Ctrl` is active low, BUSY is active high, and RST and CS are active low.
BS1 is tied to ground on RD02E, selecting 4-wire, 8-bit SPI. There is no
independent MISO signal. RC52 uses `SPI1`, explicitly remapped through the full
display constructor to SCK D10 and MOSI D9; CS remains D45.

## Runtime behavior

The sketch uses the driver's native 128-by-296 panel definition and selects a
296-by-128 landscape canvas. Cold startup displays full-screen black and white,
holding each successful frame for 1 second. These coverage frames do not change
the refresh counter. A BUSY timeout aborts the remaining startup test, powers
off RD02E and lets the next loop attempt the normal page as a full refresh.

The normal page contains static identification text above a logical window at
`(8, 72, 280, 48)`. After a successful full-screen baseline, only this window
is rewritten with the refresh count and an alternating black/white block. After
10 successful fast-window updates, the sketch performs another full refresh.
Each completed operation is followed by a 2-second rest.

`FAST WINDOW UPDATE` means that image-RAM writes are restricted to the selected
window while differential fast-refresh processing preserves the surrounding
displayed image. Available local documentation does not prove that the panel
controller drives only the selected gate region during this operation.

All drawing uses a 32-row page buffer: `128 * 32 / 8 = 512` bytes. The paging
loop clips the final page to the remaining 8 rows and does not allocate the
4,736-byte full framebuffer. The existing driver retains its panel-specific
one-byte X RAM offset and custom 153-byte fast-refresh LUT.

Each full or fast-mode setup asserts RD02E power, waits 100 ms, pulses reset low
for 10 ms and high for 10 ms, then sends software reset. Successful operations
leave RD02E powered so the controller image reference remains available. BUSY
waits time out after 60 seconds; timeout switches `VEINK_Ctrl` inactive, stops
later transfers and invalidates the example's baseline so the next cycle starts
with a complete full refresh.

## Shared-pin restrictions

Run this as a standalone example. Its GPIOs overlap soil-moisture ADC or power,
WS2812 and other display functions. Do not operate those features at the same
time.

RD02E does not isolate MCU logic signals when its switched e-paper supply is
off. After a BUSY timeout, control signals may still create a reverse-current
path; this example does not stop SPI or place every control line in a high-
impedance state.

The 10-update full-refresh interval, 2-second rest and supplied custom fast LUT
are example policies, not manufacturer guarantees for ghosting, temperature,
lifetime or safe update frequency. Validate full-screen coverage, orientation,
unchanged surrounding content and both black-to-white and white-to-black window
transitions on the actual RC32 and RC52 hardware.
