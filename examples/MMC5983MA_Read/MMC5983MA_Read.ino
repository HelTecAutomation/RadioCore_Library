#include <Arduino.h>
#include <Wire.h>
#include <RadioCore_Kit.h>

#if !RADIOCORE_HAS_MMC5983MA
#error "MMC5983MA_Read requires an onboard RadioCore MMC5983MA configuration."
#elif !RADIOCORE_HAS_SENSOR_I2C
#error "MMC5983MA_Read requires a RadioCore Sensor I2C board configuration."
#else

#include <SparkFun_MMC5983MA_Arduino_Library.h>

namespace {

constexpr float kZeroFieldCounts = 131072.0F;
constexpr float kCountsPerGauss = 16384.0F;
constexpr uint16_t kContinuousFrequencyHz = 50;
constexpr uint32_t kPrintIntervalMs = 100;

SFE_MMC5983MA magnetometer;
bool sensorReady = false;
bool continuousMode = false;
uint32_t lastPrintTime = 0;

void enableSensorHardware()
{
#if RADIOCORE_HAS_SENSOR_POWER_CTRL
  pinMode(RADIOCORE_SENSOR_POWER_CTRL_PIN, OUTPUT);
  digitalWrite(
      RADIOCORE_SENSOR_POWER_CTRL_PIN,
      RADIOCORE_SENSOR_POWER_ON);
#endif

#if RADIOCORE_HAS_SENSOR_RESET
  pinMode(RADIOCORE_SENSOR_RESET_PIN, OUTPUT);
  digitalWrite(
      RADIOCORE_SENSOR_RESET_PIN,
      RADIOCORE_SENSOR_RESET_RELEASE_LEVEL);
#endif

#if RADIOCORE_HAS_SENSOR_POWER_CTRL
  delay(RADIOCORE_SENSOR_POWER_WARMUP_MS);
#endif
}

bool beginSensorI2C()
{
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_WIFI_KIT_SERIES)
  return RADIOCORE_SENSOR_I2C_INSTANCE.begin(
      RADIOCORE_SENSOR_I2C_SDA,
      RADIOCORE_SENSOR_I2C_SCL,
      RADIOCORE_SENSOR_I2C_DEFAULT_FREQUENCY);
#elif defined(ARDUINO_ARCH_HELTEC_NRF52)
  RADIOCORE_SENSOR_I2C_INSTANCE.setPins(
      RADIOCORE_SENSOR_I2C_SDA,
      RADIOCORE_SENSOR_I2C_SCL);
  RADIOCORE_SENSOR_I2C_INSTANCE.begin();
  RADIOCORE_SENSOR_I2C_INSTANCE.setClock(
      RADIOCORE_SENSOR_I2C_DEFAULT_FREQUENCY);
  return true;
#else
#error "MMC5983MA_Read supports only the configured ESP32 and nRF52 I2C API types."
#endif
}

bool beginMagnetometer()
{
  if (!magnetometer.begin(RADIOCORE_SENSOR_I2C_INSTANCE)) {
    return false;
  }

  magnetometer.softReset();
  magnetometer.setFilterBandwidth(100);
  magnetometer.performSetOperation();
  magnetometer.enableAutomaticSetReset();

  continuousMode =
      magnetometer.setContinuousModeFrequency(kContinuousFrequencyHz);
  continuousMode &= magnetometer.enableContinuousMode();
  if (!continuousMode) {
    magnetometer.disableContinuousMode();
    Serial.println(
        F("MMC5983MA continuous mode unavailable; using single-shot reads."));
  }

  return true;
}

bool readMagneticField(float& xGauss, float& yGauss, float& zGauss)
{
  uint32_t rawX = 0;
  uint32_t rawY = 0;
  uint32_t rawZ = 0;

  const bool readSucceeded = continuousMode
      ? magnetometer.readFieldsXYZ(&rawX, &rawY, &rawZ)
      : magnetometer.getMeasurementXYZ(&rawX, &rawY, &rawZ);
  if (!readSucceeded) {
    return false;
  }

  xGauss = (static_cast<float>(rawX) - kZeroFieldCounts) / kCountsPerGauss;
  yGauss = (static_cast<float>(rawY) - kZeroFieldCounts) / kCountsPerGauss;
  zGauss = (static_cast<float>(rawZ) - kZeroFieldCounts) / kCountsPerGauss;
  return true;
}

} // namespace

void setup()
{
  Serial.begin(115200);
  delay(100);

  Serial.println();
  Serial.println(F("RadioCore onboard MMC5983MA read example"));

  enableSensorHardware();

  if (!beginSensorI2C()) {
    Serial.println(F("Sensor I2C initialization failed."));
    return;
  }

  sensorReady = beginMagnetometer();
  if (!sensorReady) {
    Serial.println(F("Onboard MMC5983MA was not found at address 0x30."));
    return;
  }

  Serial.println(F("MMC5983MA initialized at address 0x30."));
}

void loop()
{
  if (!sensorReady) {
    return;
  }

  const uint32_t now = millis();
  if (now - lastPrintTime < kPrintIntervalMs) {
    return;
  }
  lastPrintTime = now;

  float xGauss = 0.0F;
  float yGauss = 0.0F;
  float zGauss = 0.0F;
  if (!readMagneticField(xGauss, yGauss, zGauss)) {
    Serial.println(F("MMC5983MA read failed."));
    return;
  }

  Serial.print(F("Magnetic field (G) X:"));
  Serial.print(xGauss, 4);
  Serial.print(F(" Y:"));
  Serial.print(yGauss, 4);
  Serial.print(F(" Z:"));
  Serial.println(zGauss, 4);
}

#endif // RADIOCORE_HAS_MMC5983MA
