#include <Arduino.h>
#include <Wire.h>
#include <RadioCore_Kit.h>

#if !RADIOCORE_HAS_ICM42607P
#error "ICM42607P_Read requires an onboard RadioCore ICM42607P configuration."
#elif !RADIOCORE_HAS_SENSOR_I2C
#error "ICM42607P_Read requires a RadioCore Sensor I2C board configuration."
#else

#include <ICM42670P.h>

namespace {

constexpr uint8_t kIcm42607pAddresses[] = {0x68, 0x69};
constexpr uint8_t kWhoAmIRegister = 0x75;
constexpr uint8_t kIcm42607pWhoAmI = 0x60;
constexpr uint16_t kAccelerometerFrequencyHz = 50;
constexpr uint16_t kAccelerometerRangeG = 2;
constexpr uint16_t kGyroscopeFrequencyHz = 50;
constexpr uint16_t kGyroscopeRangeDps = 2000;
constexpr float kSignedSensorCounts = 32768.0F;
constexpr uint32_t kPrintIntervalMs = 100;

ICM42670 imuAtPrimaryAddress(
    RADIOCORE_SENSOR_I2C_INSTANCE,
    false,
    RADIOCORE_SENSOR_I2C_DEFAULT_FREQUENCY);
ICM42670 imuAtAlternateAddress(
    RADIOCORE_SENSOR_I2C_INSTANCE,
    true,
    RADIOCORE_SENSOR_I2C_DEFAULT_FREQUENCY);
ICM42670* imu = nullptr;
bool sensorReady = false;
uint8_t detectedAddress = 0;
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
#error "ICM42607P_Read supports only the configured ESP32 and nRF52 I2C API types."
#endif
}

bool readRegister(uint8_t address, uint8_t reg, uint8_t& value)
{
  RADIOCORE_SENSOR_I2C_INSTANCE.beginTransmission(address);
  RADIOCORE_SENSOR_I2C_INSTANCE.write(reg);
  if (RADIOCORE_SENSOR_I2C_INSTANCE.endTransmission(false) != 0) {
    return false;
  }

  if (RADIOCORE_SENSOR_I2C_INSTANCE.requestFrom(
          address,
          static_cast<uint8_t>(1)) != 1) {
    return false;
  }

  value = RADIOCORE_SENSOR_I2C_INSTANCE.read();
  return true;
}

bool detectImu()
{
  for (size_t index = 0;
       index < sizeof(kIcm42607pAddresses) /
           sizeof(kIcm42607pAddresses[0]);
       ++index) {
    const uint8_t address = kIcm42607pAddresses[index];
    uint8_t whoAmI = 0;
    if (!readRegister(address, kWhoAmIRegister, whoAmI) ||
        whoAmI != kIcm42607pWhoAmI) {
      continue;
    }

    detectedAddress = address;
    imu = address == kIcm42607pAddresses[0]
        ? &imuAtPrimaryAddress
        : &imuAtAlternateAddress;
    return true;
  }

  return false;
}

bool beginImu()
{
  if (!detectImu()) {
    Serial.println(
        F("Onboard ICM42607P was not found at address 0x68 or 0x69."));
    return false;
  }

  const int beginStatus = imu->begin();
  // ICM42670P 1.0.8 returns -3 for the ICM42607P WHO_AM_I value. The
  // register map remains compatible, and the identity was verified above.
  if (beginStatus != 0 && beginStatus != -3) {
    Serial.print(F("ICM42607P initialization failed with status "));
    Serial.println(beginStatus);
    return false;
  }

  const int accelStatus = imu->startAccel(
      kAccelerometerFrequencyHz,
      kAccelerometerRangeG);
  if (accelStatus != 0) {
    Serial.print(F("ICM42607P accelerometer start failed with status "));
    Serial.println(accelStatus);
    return false;
  }

  const int gyroStatus = imu->startGyro(
      kGyroscopeFrequencyHz,
      kGyroscopeRangeDps);
  if (gyroStatus != 0) {
    Serial.print(F("ICM42607P gyroscope start failed with status "));
    Serial.println(gyroStatus);
    return false;
  }

  return true;
}

float accelerationToG(int16_t rawValue)
{
  return static_cast<float>(rawValue) * kAccelerometerRangeG /
      kSignedSensorCounts;
}

float angularRateToDps(int16_t rawValue)
{
  return static_cast<float>(rawValue) * kGyroscopeRangeDps /
      kSignedSensorCounts;
}

} // namespace

void setup()
{
  Serial.begin(115200);
  delay(100);

  Serial.println();
  Serial.println(F("RadioCore onboard ICM42607P read example"));

  enableSensorHardware();

  if (!beginSensorI2C()) {
    Serial.println(F("Sensor I2C initialization failed."));
    return;
  }

  sensorReady = beginImu();
  if (!sensorReady) {
    return;
  }

  Serial.print(F("ICM42607P initialized at address 0x"));
  Serial.println(detectedAddress, HEX);
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

  inv_imu_sensor_event_t event = {};
  const int readStatus = imu->getDataFromRegisters(event);
  if (readStatus != 0) {
    Serial.print(F("ICM42607P read failed with status "));
    Serial.println(readStatus);
    return;
  }

  if (event.accel[0] == 0 &&
      event.accel[1] == 0 &&
      event.accel[2] == 0 &&
      event.gyro[0] == 0 &&
      event.gyro[1] == 0 &&
      event.gyro[2] == 0) {
    return;
  }

  Serial.print(F("Acceleration (g) X:"));
  Serial.print(accelerationToG(event.accel[0]), 3);
  Serial.print(F(" Y:"));
  Serial.print(accelerationToG(event.accel[1]), 3);
  Serial.print(F(" Z:"));
  Serial.println(accelerationToG(event.accel[2]), 3);

  Serial.print(F("Angular rate (deg/s) X:"));
  Serial.print(angularRateToDps(event.gyro[0]), 2);
  Serial.print(F(" Y:"));
  Serial.print(angularRateToDps(event.gyro[1]), 2);
  Serial.print(F(" Z:"));
  Serial.println(angularRateToDps(event.gyro[2]), 2);
}

#endif // RADIOCORE_HAS_ICM42607P
