/*
 * RadioCore DEPG0290BNS800 e-paper example
 *
 * Uses an RD02E driver board and the existing DEPG0290BNS800 display class
 * from heltec-eink-modules. A successful full refresh establishes the image
 * baseline before repeated fast-window updates.
 */
#include <Arduino.h>
#include <RadioCore_Kit.h>

#if !RADIOCORE_HAS_DEPG0290BNS800_DISPLAY
#error "DEPG0290BNS800_Display requires a supported RadioCore board configuration."
#else

#include <heltec-eink-modules.h>

namespace {

constexpr uint16_t kPageHeight = 32;
constexpr uint32_t kStartupFrameHoldMs = 1000UL;
constexpr uint32_t kRefreshRestMs = 2000UL;
constexpr uint8_t kFastUpdatesBeforeFull = 10;
// In landscape orientation, logical Y maps to the controller's byte-addressed
// X axis. This window is byte aligned and stays inside the 296 x 128 canvas.
constexpr uint16_t kWindowLeft = 8;
constexpr uint16_t kWindowTop = 72;
constexpr uint16_t kWindowWidth = 280;
constexpr uint16_t kWindowHeight = 48;

DEPG0290BNS800 display(
    RADIOCORE_DEPG0290BNS800_DC,
    RADIOCORE_DEPG0290BNS800_CS,
    RADIOCORE_DEPG0290BNS800_BUSY,
    RADIOCORE_DEPG0290BNS800_RST,
    RADIOCORE_DEPG0290BNS800_ENABLE,
    static_cast<SwitchType>(RADIOCORE_DEPG0290BNS800_ENABLE_ACTIVE),
    RADIOCORE_DEPG0290BNS800_MOSI,
    RADIOCORE_DEPG0290BNS800_SCK,
    kPageHeight);

uint32_t refreshCount = 0;
uint8_t fastUpdateCount = 0;
bool hasBaseline = false;

bool showStartupFrame(uint16_t color)
{
  display.fastmodeOff();
  display.fullscreen();
  if (!display.timedOut()) {
    DRAW(display) {
      display.fillScreen(color);
    }
  }
  return !display.timedOut();
}

void drawUpdateRegion(uint32_t count)
{
  // DRAW runs twice in fast mode. Never advance state inside this function.
  display.fillRect(kWindowLeft, kWindowTop, kWindowWidth, kWindowHeight, WHITE);
  display.setTextColor(BLACK);
  display.setTextSize(2);
  display.setCursor(12, 84);
  display.print(F("Refresh: "));
  display.println(count);

  if (count & 1UL) {
    display.fillRect(248, 80, 32, 32, BLACK);
  } else {
    display.drawRect(248, 80, 32, 32, BLACK);
  }
}

void drawTestPage(uint32_t count)
{
  display.drawRect(0, 0, display.width(), display.height(), BLACK);
  display.drawRect(4, 4, display.width() - 8, display.height() - 8, BLACK);

  display.setTextColor(BLACK);
  display.setTextSize(2);
  display.setCursor(12, 12);
  display.println(F("RadioCore"));

  display.setTextSize(1);
  display.setCursor(176, 16);
  display.println(F("FAST WINDOW UPDATE"));
  display.setCursor(12, 40);
  display.print(F("Board: "));
  display.println(F(RADIOCORE_DEPG0290BNS800_BOARD_NAME));
  display.setCursor(12, 54);
  display.println(F("DEPG0290BNS800  296 x 128 BW"));
  display.drawLine(8, 68, display.width() - 9, 68, BLACK);

  drawUpdateRegion(count);
}

} // namespace

void setup()
{
  Serial.begin(115200);
  delay(100);

  Serial.println();
  Serial.print(F("RadioCore DEPG0290BNS800 example: "));
  Serial.println(F(RADIOCORE_DEPG0290BNS800_BOARD_NAME));

  display.landscape();

  Serial.println(F("Startup self-test: full-screen black."));
  if (!showStartupFrame(BLACK)) {
    Serial.println(F("E-paper BUSY timeout during the black startup frame."));
    Serial.println(F("Startup self-test aborted; the next cycle will try a normal full refresh."));
    delay(kRefreshRestMs);
    return;
  }
  delay(kStartupFrameHoldMs);

  Serial.println(F("Startup self-test: full-screen white."));
  if (!showStartupFrame(WHITE)) {
    Serial.println(F("E-paper BUSY timeout during the white startup frame."));
    Serial.println(F("Startup self-test aborted; the next cycle will try a normal full refresh."));
    delay(kRefreshRestMs);
    return;
  }
  delay(kStartupFrameHoldMs);
}

void loop()
{
  const bool fullRefresh = !hasBaseline ||
      fastUpdateCount >= kFastUpdatesBeforeFull;
  const uint32_t nextCount = refreshCount + 1;
  Serial.print(fullRefresh ? F("Starting full refresh ") : F("Starting fast window update "));
  Serial.println(nextCount);

  if (fullRefresh) {
    display.fastmodeOff();
    display.fullscreen();
  } else {
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
    fastUpdateCount = 0;
    Serial.println(F("E-paper BUSY timeout; the driver board was powered down."));
    Serial.println(F("The next cycle will rebuild the image with a full refresh."));
  } else {
    refreshCount = nextCount;
    hasBaseline = true;
    if (fullRefresh) {
      fastUpdateCount = 0;
    } else {
      ++fastUpdateCount;
    }
    Serial.println(F("Refresh complete; driver-board power remains on. Resting 2 s."));
  }

  delay(kRefreshRestMs);
}

#endif // RADIOCORE_HAS_DEPG0290BNS800_DISPLAY
