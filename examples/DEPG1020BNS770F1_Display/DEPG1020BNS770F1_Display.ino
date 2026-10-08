/*
 * RadioCore DEPG1020BNS770F1 e-paper example
 *
 * Requires a heltec-eink-modules version with DEPG1020BNS770F1
 * partial-window refresh support.
 */
#include <Arduino.h>
#include <RadioCore_Kit.h>

#if !RADIOCORE_HAS_DEPG1020BNS770F1_DISPLAY
#error "DEPG1020BNS770F1_Display requires a supported RadioCore board configuration."
#else

#include <heltec-eink-modules.h>

namespace {

constexpr uint16_t kPageHeight = 32;
constexpr uint32_t kRefreshRestMs = 2000UL;
constexpr uint8_t kPartialRefreshesBeforeFull = 10;
// Byte-aligned, entirely inside 960 x 640; keep static content outside it.
constexpr uint16_t kWindowLeft = 40;
constexpr uint16_t kWindowTop = 216;
constexpr uint16_t kWindowWidth = 600;
constexpr uint16_t kWindowHeight = 56;

DEPG1020BNS770F1 display(
    RADIOCORE_DEPG1020BNS770F1_DC,
    RADIOCORE_DEPG1020BNS770F1_CS,
    RADIOCORE_DEPG1020BNS770F1_BUSY,
    RADIOCORE_DEPG1020BNS770F1_RST,
    RADIOCORE_DEPG1020BNS770F1_ENABLE,
    RADIOCORE_DEPG1020BNS770F1_ENABLE_ACTIVE,
    RADIOCORE_DEPG1020BNS770F1_MOSI,
    RADIOCORE_DEPG1020BNS770F1_SCK,
    kPageHeight);

uint32_t refreshCount = 0;
uint8_t partialRefreshCount = 0;
bool hasBaseline = false;

void drawUpdateRegion(uint32_t count)
{
  // DRAW runs twice in partial mode. Never advance state inside this function.
  display.fillRect(kWindowLeft, kWindowTop, kWindowWidth, kWindowHeight, WHITE);
  display.setTextColor(BLACK);
  display.setTextSize(2);
  display.setCursor(40, 220);
  display.print(F("Refresh count: "));
  display.println(count);
  if (count & 1UL) {
    display.fillRect(576, 220, 32, 32, BLACK);
  } else {
    display.drawRect(576, 220, 32, 32, BLACK);
  }
}

void drawTestPage(uint32_t count)
{
  display.drawRect(0, 0, display.width(), display.height(), BLACK);
  display.drawRect(8, 8, display.width() - 16, display.height() - 16, BLACK);

  display.setTextColor(BLACK);
  display.setTextSize(4);
  display.setCursor(40, 48);
  display.println(F("RadioCore"));

  display.setTextSize(2);
  display.setCursor(40, 112);
  display.print(F("Board: "));
  display.println(F(RADIOCORE_DEPG1020BNS770F1_BOARD_NAME));
  display.setCursor(40, 148);
  display.println(F("DEPG1020BNS770F1 / SSD1677"));
  display.setCursor(40, 184);
  display.println(F("960 x 640 monochrome"));
  drawUpdateRegion(count);

  constexpr int16_t blockTop = 300;
  constexpr int16_t blockHeight = 180;
  constexpr int16_t blockWidth = 180;
  display.fillRect(60, blockTop, blockWidth, blockHeight, BLACK);
  display.drawRect(260, blockTop, blockWidth, blockHeight, BLACK);
  display.fillCircle(570, blockTop + blockHeight / 2, 90, BLACK);
  display.drawCircle(790, blockTop + blockHeight / 2, 90, BLACK);

  display.drawLine(40, 540, display.width() - 40, 540, BLACK);
  display.setCursor(40, 568);
  display.println(F("Window refresh; 2 s rest; full after 10 partials"));
}

} // namespace

void setup()
{
  Serial.begin(115200);
  delay(100);

  Serial.println();
  Serial.print(F("RadioCore DEPG1020BNS770F1 example: "));
  Serial.println(F(RADIOCORE_DEPG1020BNS770F1_BOARD_NAME));
}

void loop()
{
  const bool fullRefresh = !hasBaseline ||
      partialRefreshCount >= kPartialRefreshesBeforeFull;
  const uint32_t nextCount = refreshCount + 1;
  Serial.print(fullRefresh ? F("Starting full refresh ") : F("Starting window refresh "));
  Serial.println(nextCount);

  if (fullRefresh) {
    // Always restore the full window, including after a failed partial update.
    display.fastmodeOff();
    display.fullscreen();
  } else {
    // The first full image must succeed before selecting partial mode.
    display.fastmodeOn(false);
    display.setWindow(kWindowLeft, kWindowTop, kWindowWidth, kWindowHeight);
  }
  if (!display.timedOut()) {
    DRAW(display) {
      if (fullRefresh) {
        drawTestPage(nextCount);
      } else {
        drawUpdateRegion(nextCount);
      }
    }
  }

  if (display.timedOut()) {
    hasBaseline = false;
    partialRefreshCount = 0;
    Serial.println(F("E-paper BUSY timeout; the driver board was powered down."));
    Serial.println(F("The next cycle will rebuild the image with a full refresh."));
  } else {
    refreshCount = nextCount;
    hasBaseline = true;
    if (fullRefresh) {
      partialRefreshCount = 0;
    } else {
      ++partialRefreshCount;
    }
    Serial.println(F("Refresh complete; driver-board power remains on. Resting 2 s."));
  }

  delay(kRefreshRestMs);
}

#endif // RADIOCORE_HAS_DEPG1020BNS770F1_DISPLAY
