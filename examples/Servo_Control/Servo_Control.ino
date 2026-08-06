/*
 * Controls both channels of an RS-SV01 driver. Connect PWM1/PWM2/EN to RC32
 * GPIO41/GPIO42/GPIO40, RCC6 GPIO3/GPIO4/GPIO18, or RC52
 * P1.01/P0.20/P1.06. Use a suitable servo supply and common logic ground; do
 * not power servos from GPIO.
 *
 * ESP32 boards use ESP32Servo; RC52 uses the board package's Servo library.
 * PWM pins overlap Motor_Control on all boards and NV3001B signals on RCC6.
 */
#include <Arduino.h>
#include <RadioCore.h>

#if !RADIOCORE_HAS_SERVO_CONTROL
#error "Servo_Control requires a RadioCore servo control board configuration."
#else

#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_WIFI_KIT_SERIES)
#include <ESP32Servo.h>
#elif defined(ARDUINO_ARCH_HELTEC_NRF52)
#include <Servo.h>
#endif

namespace {

constexpr int kCenterAngle = 90;
constexpr int kFirstAngle = 125;
constexpr int kSecondAngle = 60;
constexpr uint32_t kServoStepIntervalMs = 2000UL;

Servo servo1;
Servo servo2;
int currentAngle = kFirstAngle;

uint8_t servoEnableLevel(bool enabled)
{
  if (enabled) {
    return RADIOCORE_SERVO_ENABLE_ACTIVE_LEVEL;
  }

  return RADIOCORE_SERVO_ENABLE_ACTIVE_LEVEL == HIGH ? LOW : HIGH;
}

void writeServoAngle(int angle)
{
  servo1.write(angle);
  servo2.write(angle);
  Serial.print(F("Servo angle: "));
  Serial.println(angle);
}

} // namespace

void setup()
{
  Serial.begin(115200);

  digitalWrite(RADIOCORE_SERVO_ENABLE_PIN, servoEnableLevel(false));
  pinMode(RADIOCORE_SERVO_ENABLE_PIN, OUTPUT);

  servo1.attach(RADIOCORE_SERVO_PWM1_PIN);
  servo2.attach(RADIOCORE_SERVO_PWM2_PIN);
  writeServoAngle(kCenterAngle);

  digitalWrite(RADIOCORE_SERVO_ENABLE_PIN, servoEnableLevel(true));
  delay(kServoStepIntervalMs);
}

void loop()
{
  writeServoAngle(currentAngle);
  currentAngle = currentAngle == kFirstAngle ? kSecondAngle : kFirstAngle;
  delay(kServoStepIntervalMs);
}

#endif // RADIOCORE_HAS_SERVO_CONTROL
