#include <Arduino.h>
#include <RadioCore_Kit.h>

#if !RADIOCORE_HAS_SOIL_MOISTURE_ADC
#error "Soil_Moisture_Read requires a RadioCore soil moisture ADC board configuration."
#else

namespace {

constexpr uint8_t kSampleCount = 16;
constexpr uint32_t kPrintIntervalMs = 1000;
// Measured under completely dry conditions; recalibrate for each sensor.
constexpr int32_t kDryValue = 2560;
// Measured under fully wet conditions; recalibrate for each growing medium.
constexpr int32_t kWetValue = 3980;

uint32_t lastPrintTime = 0;

void enableSoilMoisturePower()
{
#if RADIOCORE_HAS_SOIL_MOISTURE_POWER_CTRL
  pinMode(RADIOCORE_SOIL_MOISTURE_POWER_CTRL_PIN, OUTPUT);
  digitalWrite(
      RADIOCORE_SOIL_MOISTURE_POWER_CTRL_PIN,
      RADIOCORE_SOIL_MOISTURE_POWER_ON);
  delay(RADIOCORE_SOIL_MOISTURE_POWER_WARMUP_MS);
#endif
}

void beginSoilMoistureAdc()
{
  pinMode(RADIOCORE_SOIL_MOISTURE_ADC_PIN, INPUT);
  analogReadResolution(12);

#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_WIFI_KIT_SERIES)
  analogSetPinAttenuation(RADIOCORE_SOIL_MOISTURE_ADC_PIN, ADC_11db);
#elif defined(ARDUINO_ARCH_HELTEC_NRF52)
  analogReference(AR_INTERNAL_3_0);
#else
#error "Soil_Moisture_Read supports only the configured ESP32 and Heltec nRF52 ADC APIs."
#endif
}

uint16_t readAverageRawValue()
{
  uint32_t rawSum = 0;
  for (uint8_t sample = 0; sample < kSampleCount; ++sample) {
    rawSum += analogRead(RADIOCORE_SOIL_MOISTURE_ADC_PIN);
  }
  return static_cast<uint16_t>(rawSum / kSampleCount);
}

uint8_t calculateMoisturePercent(uint16_t rawValue)
{
  const long moisturePercent = map(
      rawValue,
      kDryValue,
      kWetValue,
      0,
      100);
  return static_cast<uint8_t>(constrain(moisturePercent, 0L, 100L));
}

} // namespace

void setup()
{
  Serial.begin(115200);
  delay(100);

  Serial.println();
  Serial.println(F("RadioCore soil moisture read example"));

  enableSoilMoisturePower();
  beginSoilMoistureAdc();
}

void loop()
{
  const uint32_t now = millis();
  if (now - lastPrintTime < kPrintIntervalMs) {
    return;
  }
  lastPrintTime = now;

  const uint16_t rawValue = readAverageRawValue();
  const uint8_t moisturePercent = calculateMoisturePercent(rawValue);

  Serial.print(F("Raw Value: "));
  Serial.print(rawValue);
  Serial.print(F(" | Moisture: "));
  Serial.print(moisturePercent);
  Serial.println(F("%"));
}

#endif // RADIOCORE_HAS_SOIL_MOISTURE_ADC
