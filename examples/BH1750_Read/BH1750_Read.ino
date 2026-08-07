#include <Arduino.h>
#include <Wire.h>
#include <RadioCore_Kit.h>

#if !RADIOCORE_HAS_SENSOR_I2C
#error "BH1750_Read requires a RadioCore SENSOR_I2C board configuration."
#else

#include <BH1750.h>

#if !defined(BH1750_MTREG_MIN) || !defined(BH1750_MTREG_MAX)
#error "BH1750_Read requires claws/BH1750 1.3.0 or newer."
#endif

namespace {

constexpr uint8_t kBh1750Addresses[] = {0x23, 0x5C};
constexpr uint32_t kPrintIntervalMs = 1000;

BH1750 lightSensor;
bool sensorReady = false;
uint8_t detectedAddress = 0;
uint32_t lastPrintTime = 0;

void enableSensorPower()
{
#if RADIOCORE_HAS_SENSOR_POWER_CTRL
  pinMode(RADIOCORE_SENSOR_POWER_CTRL_PIN, OUTPUT);
  digitalWrite(
      RADIOCORE_SENSOR_POWER_CTRL_PIN,
      RADIOCORE_SENSOR_POWER_ON);
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
#error "BH1750_Read supports only the configured ESP32 and nRF52 I2C API types."
#endif
}

bool hasI2CAck(uint8_t address)
{
  RADIOCORE_SENSOR_I2C_INSTANCE.beginTransmission(address);
  return RADIOCORE_SENSOR_I2C_INSTANCE.endTransmission() == 0;
}

bool beginLightSensor()
{
  for (size_t index = 0;
       index < sizeof(kBh1750Addresses) / sizeof(kBh1750Addresses[0]);
       ++index) {
    const uint8_t address = kBh1750Addresses[index];

    if (!hasI2CAck(address)) {
      continue;
    }

    if (lightSensor.begin(
            BH1750::CONTINUOUS_HIGH_RES_MODE,
            address,
            &RADIOCORE_SENSOR_I2C_INSTANCE)) {
      detectedAddress = address;
      return true;
    }

    Serial.print(F("I2C ACK received at 0x"));
    Serial.print(address, HEX);
    Serial.println(F(", but BH1750 initialization failed."));
  }

  return false;
}

} // namespace

void setup()
{
  Serial.begin(115200);
  delay(100);

  Serial.println();
  Serial.println(F("RadioCore BH1750 read example"));

  enableSensorPower();

  if (!beginSensorI2C()) {
    Serial.println(F("Sensor I2C initialization failed."));
    return;
  }

  sensorReady = beginLightSensor();
  if (!sensorReady) {
    Serial.println(F("BH1750 was not found at 0x23 or 0x5C."));
    return;
  }

  Serial.print(F("BH1750 initialized at address 0x"));
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

  if (!lightSensor.measurementReady()) {
    return;
  }

  const float lux = lightSensor.readLightLevel();
  if (lux < 0.0F) {
    Serial.print(F("BH1750 read error: "));
    Serial.println(lux);
    return;
  }

  Serial.print(F("Light: "));
  Serial.print(lux, 1);
  Serial.println(F(" lx"));
}

#endif // RADIOCORE_HAS_SENSOR_I2C
