#include <Arduino.h>
#include <RadioCore.h>

#if !RADIOCORE_HAS_CO01_ADC
#error "CO01_Read requires a RadioCore CO01 ADC board configuration."
#else

namespace {

constexpr uint8_t kSampleCount = 16;
constexpr uint32_t kPrintIntervalMs = 1000;
constexpr uint32_t kTrendIntervalMs = 30000;
constexpr uint32_t kControlHighTimeMs = 60000;
constexpr uint32_t kControlLowTimeMs = 90000;
constexpr float kDividerMultiplier = 2.0F;
// Calibrate this restored 0 ppm voltage for each CO01; 1.15 V is only a default.
constexpr float kZeroPointVoltage = 1.15F;
constexpr float kConcentrationPerVolt = 150.0F;

uint32_t lastPrintTime = 0;
uint32_t lastTrendTime = 0;
float previousTrendConcentration = 0.0F;
bool trendBaselineReady = false;

void enableCo01Power()
{
#if RADIOCORE_HAS_CO01_POWER_CTRL
  pinMode(RADIOCORE_CO01_POWER_CTRL_PIN, OUTPUT);
  digitalWrite(
      RADIOCORE_CO01_POWER_CTRL_PIN,
      RADIOCORE_CO01_POWER_ON);
  delay(RADIOCORE_CO01_POWER_WARMUP_MS);
#endif
}

void runWarmupCycle()
{
  pinMode(RADIOCORE_CO01_CONTROL_PIN, OUTPUT);

  Serial.println(F("CO01 warm-up: GPIO21 HIGH for 60 s."));
  digitalWrite(RADIOCORE_CO01_CONTROL_PIN, HIGH);
  delay(kControlHighTimeMs);

  Serial.println(F("CO01 warm-up: GPIO21 LOW for 90 s."));
  digitalWrite(RADIOCORE_CO01_CONTROL_PIN, LOW);
  delay(kControlLowTimeMs);

  Serial.println(F("CO01 warm-up complete."));
}

void beginCo01Adc()
{
  pinMode(RADIOCORE_CO01_ADC_PIN, INPUT);
  analogReadResolution(12);

#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_WIFI_KIT_SERIES)
  analogSetPinAttenuation(RADIOCORE_CO01_ADC_PIN, ADC_11db);
#elif defined(ARDUINO_ARCH_HELTEC_NRF52)
  analogReference(AR_INTERNAL_3_0);
#else
#error "CO01_Read supports only the configured ESP32 and Heltec nRF52 ADC APIs."
#endif
}

float readAdcMillivolts()
{
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_WIFI_KIT_SERIES)
  uint32_t millivoltSum = 0;
  for (uint8_t sample = 0; sample < kSampleCount; ++sample) {
    millivoltSum += analogReadMilliVolts(RADIOCORE_CO01_ADC_PIN);
  }
  return static_cast<float>(millivoltSum) / kSampleCount;
#elif defined(ARDUINO_ARCH_HELTEC_NRF52)
  uint32_t rawSum = 0;
  for (uint8_t sample = 0; sample < kSampleCount; ++sample) {
    rawSum += analogRead(RADIOCORE_CO01_ADC_PIN);
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
  Serial.print(F("CO01 voltage: "));
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
  Serial.println(F("RadioCore CO01 concentration example"));
  enableCo01Power();
  runWarmupCycle();
  beginCo01Adc();
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

#endif // RADIOCORE_HAS_CO01_ADC
