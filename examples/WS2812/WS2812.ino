/*
 * Drives 30 WS2812 pixels (GRB, 800 kHz) through RC32 GPIO5, RCC6 GPIO2, or
 * RC52 P0.09. Change LED_COUNT for a different strip length.
 *
 * Use a supply sized for the strip and connect its ground to board ground; do
 * not power the strip from a GPIO. The data pin overlaps NV3001B backlight or
 * enable and soil-moisture power signals, so run this as a standalone example.
 */
#define LED_COUNT 30

#include <Arduino.h>
#include <RadioCore_Kit.h>

#if !RADIOCORE_HAS_WS2812
#error "WS2812 requires a RadioCore WS2812 board configuration."
#else

#include <Adafruit_NeoPixel.h>

namespace {

constexpr uint8_t kBrightness = 128;
constexpr uint32_t kBreathDurationMs = 6000;

constexpr uint8_t kColorTable[][3] = {
    {255, 255, 50},
    {50, 220, 255},
    {255, 255, 255}};

Adafruit_NeoPixel strip(
    LED_COUNT,
    RADIOCORE_WS2812_DATA_PIN,
    NEO_GRB + NEO_KHZ800);

uint8_t colorIndex = 0;
uint32_t breathStartTime = 0;

void showBreathingColor(uint32_t elapsed)
{
  const uint32_t halfDuration = kBreathDurationMs / 2;
  uint8_t brightness;
  if (elapsed <= halfDuration) {
    brightness = static_cast<uint8_t>(255UL * elapsed / halfDuration);
  } else {
    brightness = static_cast<uint8_t>(
        255UL * (kBreathDurationMs - elapsed) / halfDuration);
  }

  const uint8_t red = static_cast<uint8_t>(
      kColorTable[colorIndex][0] * brightness / 255);
  const uint8_t green = static_cast<uint8_t>(
      kColorTable[colorIndex][1] * brightness / 255);
  const uint8_t blue = static_cast<uint8_t>(
      kColorTable[colorIndex][2] * brightness / 255);

  strip.clear();
  for (uint16_t pixel = 0; pixel < LED_COUNT; ++pixel) {
    strip.setPixelColor(pixel, strip.Color(red, green, blue));
  }
  strip.show();
}

} // namespace

void setup()
{
  Serial.begin(115200);
  delay(100);

  Serial.println();
  Serial.println(F("RadioCore WS2812 breathing example"));

  strip.begin();
  strip.setBrightness(kBrightness);
  strip.clear();
  strip.show();
  breathStartTime = millis();
}

void loop()
{
  const uint32_t now = millis();
  uint32_t elapsed = now - breathStartTime;

  if (elapsed >= kBreathDurationMs) {
    const uint32_t completedCycles = elapsed / kBreathDurationMs;
    colorIndex = static_cast<uint8_t>(
        (colorIndex + completedCycles) %
        (sizeof(kColorTable) / sizeof(kColorTable[0])));
    breathStartTime += completedCycles * kBreathDurationMs;
    elapsed %= kBreathDurationMs;
  }

  showBreathingColor(elapsed);
}

#endif // RADIOCORE_HAS_WS2812
