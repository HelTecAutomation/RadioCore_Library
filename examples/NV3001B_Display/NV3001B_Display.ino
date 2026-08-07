/*
 * RadioCore NV3001B display example
 *
 * Requires the NV3001B-enabled Arduino_GFX fork:
 * https://github.com/Quency-D/Arduino_GFX
 */
#include <Arduino.h>
#include <RadioCore_Kit.h>

#if !RADIOCORE_HAS_NV3001B_DISPLAY
#error "NV3001B_Display requires a RadioCore NV3001B board configuration."
#else

#include <Arduino_GFX_Library.h>

namespace {

constexpr uint8_t kDisplayRotation = 3;
constexpr uint16_t kPanelWidth = 128;
constexpr uint16_t kPanelHeight = 220;
constexpr uint32_t kCounterIntervalMs = 1000;

#if RADIOCORE_NV3001B_USE_ESP32_SPI
Arduino_ESP32SPI displayBus(
    RADIOCORE_NV3001B_DC,
    RADIOCORE_NV3001B_CS,
    RADIOCORE_NV3001B_SCK,
    RADIOCORE_NV3001B_MOSI,
    RADIOCORE_NV3001B_MISO,
    RADIOCORE_NV3001B_SPI_HOST);
#elif RADIOCORE_NV3001B_USE_HARDWARE_SPI
Arduino_HWSPI displayBus(
    RADIOCORE_NV3001B_DC,
    RADIOCORE_NV3001B_CS,
    &RADIOCORE_NV3001B_SPI_INSTANCE,
    true);
#elif RADIOCORE_NV3001B_USE_SOFTWARE_SPI
Arduino_SWSPI displayBus(
    RADIOCORE_NV3001B_DC,
    RADIOCORE_NV3001B_CS,
    RADIOCORE_NV3001B_SCK,
    RADIOCORE_NV3001B_MOSI,
    RADIOCORE_NV3001B_MISO);
#else
#error "NV3001B_Display has no configured display transport."
#endif

Arduino_NV3001B display(
    &displayBus,
    RADIOCORE_NV3001B_RST,
    kDisplayRotation,
    true,
    kPanelWidth,
    kPanelHeight,
    0,
    0,
    0,
    0);

bool displayReady = false;
uint32_t lastCounterUpdateMs = 0;
uint32_t counter = 0;

void beginDisplayTransport()
{
#if RADIOCORE_NV3001B_USE_HARDWARE_SPI
  RADIOCORE_NV3001B_SPI_INSTANCE.setPins(
      RADIOCORE_NV3001B_SPI_MISO,
      RADIOCORE_NV3001B_SCK,
      RADIOCORE_NV3001B_MOSI);
#endif
}

void enableDisplayPower()
{
  pinMode(RADIOCORE_NV3001B_ENABLE, OUTPUT);
  digitalWrite(
      RADIOCORE_NV3001B_ENABLE,
      RADIOCORE_NV3001B_ENABLE_ACTIVE);

  pinMode(RADIOCORE_NV3001B_BACKLIGHT, OUTPUT);
  digitalWrite(
      RADIOCORE_NV3001B_BACKLIGHT,
      !RADIOCORE_NV3001B_BACKLIGHT_ACTIVE);
  delay(20);
}

void drawStaticContent()
{
  display.fillScreen(RGB565_BLACK);

  display.setTextColor(RGB565_WHITE);
  display.setTextSize(2);
  display.setCursor(8, 8);
  display.println(F("RadioCore"));

  display.setTextSize(1);
  display.setCursor(8, 34);
  display.print(F("Board: "));
  display.println(F(RADIOCORE_NV3001B_BOARD_NAME));
  display.setCursor(8, 48);
  display.println(F("NV3001B 128x220"));

  constexpr uint16_t colors[] = {
      RGB565_RED,
      RGB565_GREEN,
      RGB565_BLUE,
      RGB565_CYAN,
      RGB565_MAGENTA,
      RGB565_YELLOW,
      RGB565_WHITE};
  constexpr int16_t blockWidth = 26;
  constexpr int16_t blockGap = 4;
  for (size_t index = 0; index < sizeof(colors) / sizeof(colors[0]); ++index) {
    display.fillRect(
        8 + index * (blockWidth + blockGap),
        64,
        blockWidth,
        16,
        colors[index]);
  }
}

void drawCounter()
{
  display.fillRect(0, 90, display.width(), 38, RGB565_BLACK);
  display.setTextColor(RGB565_ORANGE);
  display.setTextSize(2);
  display.setCursor(8, 98);
  display.print(F("Count: "));
  display.println(counter);
}

} // namespace

void setup()
{
  Serial.begin(115200);
  delay(100);

  Serial.println();
  Serial.print(F("RadioCore NV3001B display example: "));
  Serial.println(F(RADIOCORE_NV3001B_BOARD_NAME));

  enableDisplayPower();
  beginDisplayTransport();

  if (!display.begin(RADIOCORE_NV3001B_SPI_FREQUENCY)) {
    Serial.println(F("NV3001B initialization failed."));
    return;
  }

  display.setRotation(kDisplayRotation);
  drawStaticContent();
  drawCounter();
  digitalWrite(
      RADIOCORE_NV3001B_BACKLIGHT,
      RADIOCORE_NV3001B_BACKLIGHT_ACTIVE);
  displayReady = true;
}

void loop()
{
  if (!displayReady) {
    return;
  }

  const uint32_t now = millis();
  if (now - lastCounterUpdateMs < kCounterIntervalMs) {
    return;
  }
  lastCounterUpdateMs = now;

  ++counter;
  drawCounter();
}

#endif // RADIOCORE_HAS_NV3001B_DISPLAY
