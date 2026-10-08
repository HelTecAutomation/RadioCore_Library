/*
 * Reads the onboard rotary encoder on Heltec RC32 and RC52 through the
 * TCA6408 I2C expander. RC32 uses Sensor I2C on GPIO21/GPIO18 with shared
 * peripheral power on GPIO46; RC52 uses P1.11/P0.02 with shared peripheral
 * power on P0.12. Turning the encoder prints Previous/Next and a signed count.
 *
 * The board-designated user button is reported independently from rotation:
 * GPIO0 on RC32 and P1.10 on RC52. It is not treated as an encoder push event.
 * Avoid holding RC32 GPIO0 low while resetting or uploading because it is also
 * a boot-strapping pin.
 */
#include <Arduino.h>
#include <Wire.h>
#include <RadioCore_Kit.h>

#if !RADIOCORE_HAS_ROTARY_ENCODER
#error "Rotary_Encoder requires a RadioCore rotary encoder board configuration."
#elif !RADIOCORE_HAS_SENSOR_I2C
#error "Rotary_Encoder requires a RadioCore Sensor I2C board configuration."
#else

namespace {

constexpr uint8_t kInputRegister = 0x00;
constexpr uint8_t kPolarityRegister = 0x02;
constexpr uint8_t kConfigRegister = 0x03;
constexpr uint8_t kRotaryMask =
    RADIOCORE_ROTARY_ENCODER_PHASE_A_MASK |
    RADIOCORE_ROTARY_ENCODER_PHASE_B_MASK;
constexpr uint32_t kRotaryEventIntervalMs = 5;
constexpr uint32_t kEncoderRetryIntervalMs = 1000;
constexpr uint32_t kButtonDebounceMs = 20;

enum class RotaryEvent : uint8_t {
  None,
  Previous,
  Next,
};

bool encoderReady = false;
bool encoderWasReady = false;
bool activeLowPhase = false;
uint8_t inputState = kRotaryMask;
uint32_t lastRotaryEventTime = 0;
uint32_t nextEncoderRetryTime = 0;
int32_t rotaryCount = 0;

#if RADIOCORE_HAS_USER_BUTTON
uint8_t buttonRawState = HIGH;
uint8_t buttonStableState = HIGH;
uint32_t buttonChangedAt = 0;
#endif

bool timeReached(uint32_t now, uint32_t deadline)
{
  return static_cast<int32_t>(now - deadline) >= 0;
}

void enableSharedPower()
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
#error "Rotary_Encoder supports only the configured ESP32 and nRF52 I2C API types."
#endif
}

bool writeExpanderRegister(uint8_t reg, uint8_t value)
{
  RADIOCORE_SENSOR_I2C_INSTANCE.beginTransmission(
      RADIOCORE_ROTARY_ENCODER_I2C_ADDRESS);
  RADIOCORE_SENSOR_I2C_INSTANCE.write(reg);
  RADIOCORE_SENSOR_I2C_INSTANCE.write(value);
  return RADIOCORE_SENSOR_I2C_INSTANCE.endTransmission() == 0;
}

bool readExpanderRegister(uint8_t reg, uint8_t& value)
{
  RADIOCORE_SENSOR_I2C_INSTANCE.beginTransmission(
      RADIOCORE_ROTARY_ENCODER_I2C_ADDRESS);
  RADIOCORE_SENSOR_I2C_INSTANCE.write(reg);
  if (RADIOCORE_SENSOR_I2C_INSTANCE.endTransmission(false) != 0) {
    return false;
  }

  if (RADIOCORE_SENSOR_I2C_INSTANCE.requestFrom(
          RADIOCORE_ROTARY_ENCODER_I2C_ADDRESS,
          static_cast<uint8_t>(1)) != 1) {
    return false;
  }

  value = RADIOCORE_SENSOR_I2C_INSTANCE.read();
  return true;
}

bool updateExpanderBits(uint8_t reg, uint8_t setMask, uint8_t clearMask)
{
  uint8_t currentValue = 0;
  if (!readExpanderRegister(reg, currentValue)) {
    return false;
  }

  const uint8_t newValue = (currentValue | setMask) & ~clearMask;
  return newValue == currentValue || writeExpanderRegister(reg, newValue);
}

bool beginEncoder()
{
  encoderReady = false;
  activeLowPhase = false;
  inputState = kRotaryMask;

  if (!updateExpanderBits(kPolarityRegister, 0, kRotaryMask) ||
      !updateExpanderBits(kConfigRegister, kRotaryMask, 0)) {
    return false;
  }

  uint8_t state = 0;
  if (!readExpanderRegister(kInputRegister, state)) {
    return false;
  }

  inputState = state & kRotaryMask;
  encoderReady = true;
  return true;
}

bool tryBeginEncoder(uint32_t now)
{
  if (!beginSensorI2C() || !beginEncoder()) {
    encoderReady = false;
    nextEncoderRetryTime = now + kEncoderRetryIntervalMs;
    Serial.println(F("Rotary encoder unavailable; retrying in 1 s."));
    return false;
  }

  Serial.println(
      encoderWasReady ? F("Rotary encoder recovered.")
                      : F("Rotary encoder ready."));
  encoderWasReady = true;
  return true;
}

RotaryEvent handleTransition(uint8_t newState, uint32_t now)
{
  const uint8_t changed = (inputState ^ newState) & kRotaryMask;
  const bool aLow =
      (newState & RADIOCORE_ROTARY_ENCODER_PHASE_A_MASK) == 0;
  const bool bLow =
      (newState & RADIOCORE_ROTARY_ENCODER_PHASE_B_MASK) == 0;
  RotaryEvent event = RotaryEvent::None;

  if (!aLow && !bLow) {
    activeLowPhase = false;
  }

  if (!activeLowPhase &&
      (changed & RADIOCORE_ROTARY_ENCODER_PHASE_A_MASK) &&
      aLow && !bLow) {
    event = RotaryEvent::Previous;
    activeLowPhase = true;
  } else if (!activeLowPhase &&
             (changed & RADIOCORE_ROTARY_ENCODER_PHASE_B_MASK) &&
             bLow && !aLow) {
    event = RotaryEvent::Next;
    activeLowPhase = true;
  }

  if (event == RotaryEvent::None && !activeLowPhase &&
      (changed & RADIOCORE_ROTARY_ENCODER_PHASE_A_MASK) &&
      !aLow && bLow) {
    event = RotaryEvent::Previous;
  }

  if (event == RotaryEvent::None && !activeLowPhase &&
      (changed & RADIOCORE_ROTARY_ENCODER_PHASE_B_MASK) &&
      !bLow && aLow) {
    event = RotaryEvent::Next;
  }

  if (event == RotaryEvent::None ||
      now - lastRotaryEventTime < kRotaryEventIntervalMs) {
    return RotaryEvent::None;
  }

  lastRotaryEventTime = now;
  return event;
}

RotaryEvent pollEncoder(uint32_t now, bool& readSucceeded)
{
  uint8_t newState = 0;
  readSucceeded = readExpanderRegister(kInputRegister, newState);
  if (!readSucceeded) {
    return RotaryEvent::None;
  }

  newState &= kRotaryMask;
  const RotaryEvent event = handleTransition(newState, now);
  inputState = newState;
  return event;
}

void printRotaryEvent(RotaryEvent event)
{
  if (event == RotaryEvent::Previous) {
    --rotaryCount;
    Serial.print(F("Previous, count: "));
  } else if (event == RotaryEvent::Next) {
    ++rotaryCount;
    Serial.print(F("Next, count: "));
  } else {
    return;
  }

  Serial.println(rotaryCount);
}

#if RADIOCORE_HAS_USER_BUTTON
void beginUserButton(uint32_t now)
{
  pinMode(
      RADIOCORE_USER_BUTTON_PIN,
      RADIOCORE_USER_BUTTON_PIN_MODE);
  buttonRawState = digitalRead(RADIOCORE_USER_BUTTON_PIN);
  buttonStableState = buttonRawState;
  buttonChangedAt = now;
}

void pollUserButton(uint32_t now)
{
  const uint8_t state = digitalRead(RADIOCORE_USER_BUTTON_PIN);
  if (state != buttonRawState) {
    buttonRawState = state;
    buttonChangedAt = now;
  }

  if (buttonRawState == buttonStableState ||
      now - buttonChangedAt < kButtonDebounceMs) {
    return;
  }

  buttonStableState = buttonRawState;
  if (buttonStableState == RADIOCORE_USER_BUTTON_ACTIVE_LEVEL) {
    Serial.println(F("User button pressed"));
  }
}
#endif

} // namespace

void setup()
{
  Serial.begin(115200);
  delay(100);

  Serial.println();
  Serial.println(F("RadioCore rotary encoder example"));

  const uint32_t now = millis();
#if RADIOCORE_HAS_USER_BUTTON
  beginUserButton(now);
#endif
  enableSharedPower();
  tryBeginEncoder(millis());
}

void loop()
{
  const uint32_t now = millis();

#if RADIOCORE_HAS_USER_BUTTON
  pollUserButton(now);
#endif

  if (!encoderReady) {
    if (timeReached(now, nextEncoderRetryTime)) {
      tryBeginEncoder(now);
    }
    return;
  }

  bool readSucceeded = false;
  const RotaryEvent event = pollEncoder(now, readSucceeded);
  if (!readSucceeded) {
    encoderReady = false;
    nextEncoderRetryTime = now + kEncoderRetryIntervalMs;
    Serial.println(F("Rotary encoder read failed; retrying in 1 s."));
    return;
  }

  printRotaryEvent(event);
}

#endif // RADIOCORE_HAS_ROTARY_ENCODER
