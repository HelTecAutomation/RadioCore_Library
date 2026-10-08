/*
 * RadioCore E0213A367 e-paper example
 *
 * Uses the RD02E driver board and the E0213A367 display class from
 * heltec-eink-modules. The example performs a full refresh, keeps the driver
 * powered for 60 seconds, then repeats with a new refresh count.
 */
#include <Arduino.h>
#include <RadioCore_Kit.h>

#if !RADIOCORE_HAS_E0213A367_DISPLAY
#error "E0213A367_Display requires a supported RadioCore board configuration."
#else

#include <heltec-eink-modules.h>

namespace {

constexpr uint16_t kPageHeight = 32;
constexpr uint32_t kRefreshRestMs = 60000UL;

E0213A367 display(
    RADIOCORE_E0213A367_DC,
    RADIOCORE_E0213A367_CS,
    RADIOCORE_E0213A367_BUSY,
    RADIOCORE_E0213A367_RST,
    RADIOCORE_E0213A367_ENABLE,
    RADIOCORE_E0213A367_ENABLE_ACTIVE,
    RADIOCORE_E0213A367_MOSI,
    RADIOCORE_E0213A367_SCK,
    kPageHeight);

uint32_t refreshCount = 0;

void drawTestPage()
{
  display.drawRect(0, 0, display.width(), display.height(), BLACK);
  display.drawRect(3, 3, display.width() - 6, display.height() - 6, BLACK);

  display.setTextColor(BLACK);
  display.setTextSize(2);
  display.setCursor(10, 14);
  display.println(F("RadioCore"));

  display.setTextSize(1);
  display.setCursor(10, 46);
  display.println(F(RADIOCORE_E0213A367_BOARD_NAME));
  display.setCursor(10, 62);
  display.println(F("E0213A367"));
  display.setCursor(10, 78);
  display.println(F("122 x 250 BW"));
  display.setCursor(10, 94);
  display.print(F("Refresh: "));
  display.println(refreshCount);

  display.fillRect(12, 122, 38, 38, BLACK);
  display.drawRect(70, 122, 38, 38, BLACK);
  display.fillCircle(31, 190, 19, BLACK);
  display.drawCircle(89, 190, 19, BLACK);

  display.drawLine(10, 222, display.width() - 10, 222, BLACK);
  display.setCursor(10, 232);
  display.println(F("Full refresh"));
}

} // namespace

void setup()
{
  Serial.begin(115200);
  delay(100);

  Serial.println();
  Serial.print(F("RadioCore E0213A367 example: "));
  Serial.println(F(RADIOCORE_E0213A367_BOARD_NAME));
}

void loop()
{
  ++refreshCount;
  Serial.print(F("Starting full refresh "));
  Serial.println(refreshCount);

  // Reasserts RD02E power, hard-resets the panel and loads full-refresh mode.
  display.fastmodeOff();
  if (!display.timedOut()) {
    DRAW(display) {
      drawTestPage();
    }
  }

  if (display.timedOut()) {
    Serial.println(F("E-paper BUSY timeout; RD02E power is off."));
  } else {
    Serial.println(F("Refresh complete; RD02E power remains on."));
  }

  delay(kRefreshRestMs);
}

#endif // RADIOCORE_HAS_E0213A367_DISPLAY
