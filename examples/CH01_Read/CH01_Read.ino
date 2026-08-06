#include <Arduino.h>
#include <RadioCore.h>

#if !RADIOCORE_HAS_CH01_ADC
#error "CH01_Read requires a RadioCore CH01 ADC board configuration."
#else

namespace {

constexpr uint8_t kSampleCount = 16;
constexpr uint32_t kPrintIntervalMs = 1000;
constexpr uint32_t kTrendIntervalMs = 30000;
constexpr float kDividerMultiplier = 2.0F;
// Calibrate this restored 0 ppm voltage for each CH01; 1.15 V is only a default.
constexpr float kZeroPointVoltage = 1.15F;
constexpr float kConcentrationPerVolt = 5000.0F;

uint32_t lastPrintTime = 0;
uint32_t lastTrendTime = 0;
float previousTrendConcentration = 0.0F;
bool trendBaselineReady = false;

void enableCh01Power()
{
#if RADIOCORE_HAS_CH01_POWER_CTRL
  pinMode(RADIOCORE_CH01_POWER_CTRL_PIN, OUTPUT);
  digitalWrite(
      RADIOCORE_CH01_POWER_CTRL_PIN,
      RADIOCORE_CH01_POWER_ON);
  delay(RADIOCORE_CH01_POWER_WARMUP_MS);
#endif
}

void beginCh01Adc()
{
  pinMode(RADIOCORE_CH01_ADC_PIN, INPUT);
  analogReadResolution(12);

#if defined(ARDUINO_ARCH_HELTEC_NRF52)
  analogReference(AR_INTERNAL_3_0);
#elif !defined(ARDUINO_ARCH_ESP32) && \
    !defined(ARDUINO_ARCH_WIFI_KIT_SERIES)
#error "CH01_Read supports only the configured ESP32 and Heltec nRF52 ADC APIs."
#endif
}

float readAdcMillivolts()
{
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_WIFI_KIT_SERIES)
  uint32_t millivoltSum = 0;
  for (uint8_t sample = 0; sample < kSampleCount; ++sample) {
    millivoltSum += analogReadMilliVolts(RADIOCORE_CH01_ADC_PIN);
  }
  return static_cast<float>(millivoltSum) / kSampleCount;
#elif defined(ARDUINO_ARCH_HELTEC_NRF52)
  uint32_t rawSum = 0;
  for (uint8_t sample = 0; sample < kSampleCount; ++sample) {
    rawSum += analogRead(RADIOCORE_CH01_ADC_PIN);
  }
  const float averageRaw = static_cast<float>(rawSum) / kSampleCount;
  return averageRaw * 3000.0F / 4096.0F;
#endif
}

float calculateConcentration(float restoredVoltage)
{
  const float concentration =
      (restoredVoltage - kZeroPointVoltage) * kConcentrationPerVolt;
  return concentration > 0.0F ? concentration : 0.0F;
}

void printReading(float restoredVoltage, float concentration)
{
  Serial.print(F("CH01 voltage: "));
  Serial.print(restoredVoltage, 3);
  Serial.print(F(" V, concentration: "));
  Serial.print(concentration, 1);
  Serial.println(F(" ppm"));
}

void printTrend(float previousConcentration, float currentConcentration)
{
  Serial.print(F("30 s trend: "));
  if (currentConcentration < previousConcentration) {
    Serial.print(F("decreased"));
  } else if (currentConcentration > previousConcentration) {
    Serial.print(F("increased"));
  } else {
    Serial.print(F("unchanged"));
  }

  Serial.print(F(" (previous: "));
  Serial.print(previousConcentration, 1);
  Serial.print(F(" ppm, current: "));
  Serial.print(currentConcentration, 1);
  Serial.println(F(" ppm)"));
}

} // namespace

void setup()
{
  Serial.begin(115200);
  delay(100);

  Serial.println();
  Serial.println(F("RadioCore CH01 concentration example"));
  enableCh01Power();
  beginCh01Adc();
}

void loop()
{
  const uint32_t now = millis();
  if (now - lastPrintTime < kPrintIntervalMs) {
    return;
  }
  lastPrintTime = now;

  const float adcMillivolts = readAdcMillivolts();
  const float restoredVoltage =
      adcMillivolts * kDividerMultiplier / 1000.0F;
  const float concentration = calculateConcentration(restoredVoltage);

  printReading(restoredVoltage, concentration);

  if (!trendBaselineReady) {
    previousTrendConcentration = concentration;
    lastTrendTime = now;
    trendBaselineReady = true;
    return;
  }

  if (now - lastTrendTime >= kTrendIntervalMs) {
    printTrend(previousTrendConcentration, concentration);
    previousTrendConcentration = concentration;
    lastTrendTime = now;
  }
}

#endif // RADIOCORE_HAS_CH01_ADC
