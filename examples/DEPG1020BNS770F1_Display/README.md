# DEPG1020BNS770F1 E-Paper Display

## Purpose

This example drives a DEPG1020BNS770F1 960-by-640 monochrome e-paper panel
through an RD02E SSD1677 driver board. It uses 32-row paged drawing, builds a
full-screen baseline, then updates a small window with the manufacturer's
partial-refresh waveform. At cold startup it first shows full-screen black and
white test frames. It rests for 2 seconds after each completed normal refresh
and performs a full-screen refresh after every 10 successful partial updates.

## Requirements

- RadioCore RC32 or RC52 board configuration
- A DEPG1020BNS770F1 driver board that includes the panel power and high-voltage
  circuitry; do not connect the bare 24-pin panel directly to the MCU
- A `heltec-eink-modules` version with DEPG1020BNS770F1 partial-refresh support

Install the updated `heltec-eink-modules` library manually in the Arduino
sketchbook `libraries` directory. The currently published Library Manager
version does not contain this panel driver.

## RD02E wiring

RD02E P1 mates pin-for-pin with RC32 P3 or RC52 P1. The table uses the updated
RD02E signal assignment and the RadioCore connector mappings; it is not the
RadioCore TFT SPI mapping.

| Signal | RD02E P1 | Heltec RC32 P3 | Heltec RC52 P1 |
| --- | ---: | ---: | ---: |
| RST/RES | 10 | GPIO16 | P0.28 (28) |
| BUSY | 24 | GPIO38 | P1.02 (34) |
| DC | 8 | GPIO17 | P0.30 (30) |
| EN/VEINK_Ctrl | 26 | GPIO39 | P1.04 (36) |
| CS | 2 | GPIO6 | P1.13 (45) |
| SCK/SCL | 1 | GPIO4 | P0.10 (10) |
| MOSI/SDA | 3 | GPIO5 | P0.09 (9) |

`VEINK_Ctrl` is active low: pulling it low turns on the RD02E P-channel power
switch. BUSY is active high, while RST and CS are active low. BS1 is tied to
ground on RD02E, fixing the panel in 4-wire, 8-bit SPI mode. There is no separate
MISO signal; SDA is only used as MOSI by this write-only driver.

RC52 uses `SPI1`, explicitly remapped to SCK D10 and MOSI D9 by the display
constructor. Do not rely on the board variant's TFT SPI defaults.

The example owns these connector pins while it runs. Do not simultaneously use
the conflicting soil-moisture ADC/power, WS2812, TFT,
or other connector functions. Check the PCB and RD02E silkscreen pin-1 marks
before applying power so the board-to-board connector is not inserted mirrored.

## Behavior and limitations

At cold startup, the sketch performs one full-screen black refresh and one
full-screen white refresh, holding each completed frame for 1 second. It then
draws the identification page and test geometry over the complete 960-by-640
screen. The startup frames do not increment the displayed refresh counter and
are not repeated by periodic full refreshes or timeout recovery. If either
startup frame reaches the 60-second BUSY timeout, the driver board is powered
down, the remaining startup frame is skipped, and the next loop cycle attempts
the normal identification page as a full refresh.

The normal page keeps `RadioCore` at the upper left and displays
`PARTIAL-WINDOW REFRESH` at the upper right so the top edge is visibly exercised.
Both labels are static full-refresh content. Subsequent partial updates select
the byte-aligned rectangle `(40, 216, 600, 56)`, clear that rectangle to white,
and redraw only the refresh counter and an alternating black/white test block.
The border, board name, header, and other geometry remain unchanged. Coordinates
are absolute screen coordinates, not relative to the window.

The sketch selects `Flip::VERTICAL` before the startup test. With the verified
panel scan direction, this keeps text readable while rotating the physical
layout by 180 degrees, so the former upper-left content appears at the lower
right. The same mapping is applied to full-screen and partial-window updates.

All startup and normal drawing uses a 3,840-byte page buffer
(`960 * 32 / 8`), not a 76,800-byte full framebuffer. Partial mode runs the
`DRAW` body twice through the shared library paging loop; the DEPG1020 driver
skips hardware writes on the second pass. The counter and block state stay
constant throughout both passes and advance only after a successful refresh.
Every 10 partial updates, the sketch explicitly restores `fullscreen()` and
refreshes the full image before continuing.

Both full and partial refreshes now keep RD02E powered, with active-low
`VEINK_Ctrl` held low. Normal completion does not send deep sleep or turn power
off. This is a driver-wide behavior change, not an example-only option, and
increases standby power consumption. No public sleep/power-off API is added.
The driver waits at most 60 seconds for BUSY to return low. On timeout it
disables driver-board power and stops subsequent transfers. After the 2-second
rest, the next cycle powers on, resets, and rebuilds a full-screen baseline;
failed updates do not advance the displayed counter or the partial-update count.

The port uses the supplied top-level SSD1677 demo's `WF_PARTIAL` (105 LUT bytes
plus five voltage parameters), `0x37` configuration, border `0x80`, and `0xCF`
activation. Full-refresh drawing writes the same paged image to both `0x24`
and `0x26`, restoring the page address counters before the second write to
initialize a consistent full-image reference. Partial windows still use
only `0x24`, with the second hardware pass skipped. The sketch does not detect
changed pixels automatically or use the nested SSD1685 project or OTP readback.
On 2026-10-08, the user confirmed normal display after adding the full-refresh
`0x26` write to resolve the reported window-external noise on the RC32 setup.
Temporary diagnostic logging and its API have been removed.
The existing Y-decreasing address mapping
is retained: the vendor window example has inconsistent Y direction and an
out-of-bounds demonstration rectangle, so its coordinates are not copied.

Partial refresh remains experimental beyond this initial user-confirmed RC32
test. RC52 hardware, long-term residual images, BUSY duration, and timeout
recovery still need validation; check window position, unchanged surrounding
content, and both black-to-white and white-to-black transitions on each setup.
Ten partial updates between full refreshes is an adjustable example policy, not
a manufacturer-guaranteed limit. Grayscale and Turbo mode are not implemented.
RD02E has no isolation between the MCU control signals and the powered-down
SSD1677 domain. This implementation does not place the SPI and control pins in
high impedance after disabling `VDD_EINK`; hardware backfeed remains unverified.
