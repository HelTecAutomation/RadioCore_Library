/*
 * Connect an external motor driver's IN1/IN2 to RC32 GPIO41/GPIO42, RCC6
 * GPIO3/GPIO4, or RC52 P1.01/P0.20. Use a suitable motor supply and connect
 * driver logic ground to board ground; never connect a motor directly to GPIO.
 *
 * Outputs stay low for 10 s, run IN1 for 3 s, stop for 17 s, then use the IN2
 * direction for later runs. RCC6 GPIO3/GPIO4 overlap NV3001B DC/SCK.
 */
#include <Arduino.h>
#include <RadioCore.h>

#if !RADIOCORE_HAS_MOTOR_CONTROL
#error "Motor_Control requires a RadioCore motor control board configuration."
#else

namespace {

constexpr uint32_t kInitialWaitMs = 10000UL;
constexpr uint32_t kRunDurationMs = 3000UL;
constexpr uint32_t kStopDurationMs = 17000UL;

enum MotorPhase {
  WAIT_INIT,
  RUN,
  STOP
};

MotorPhase phase = WAIT_INIT;
uint32_t phaseStartTime = 0;

void setMotorOutputs(uint8_t in1Level, uint8_t in2Level)
{
  digitalWrite(RADIOCORE_MOTOR_IN1_PIN, in1Level);
  digitalWrite(RADIOCORE_MOTOR_IN2_PIN, in2Level);
}

} // namespace

void setup()
{
  Serial.begin(115200);

  pinMode(RADIOCORE_MOTOR_IN1_PIN, OUTPUT);
  pinMode(RADIOCORE_MOTOR_IN2_PIN, OUTPUT);

  setMotorOutputs(LOW, LOW);
  phaseStartTime = millis();
}

void loop()
{
  const uint32_t currentMillis = millis();

  switch (phase) {
    case WAIT_INIT:
      if (currentMillis - phaseStartTime >= kInitialWaitMs) {
        setMotorOutputs(HIGH, LOW);
        Serial.println(F("Motor running: IN1 direction"));
        phase = RUN;
        phaseStartTime = currentMillis;
      }
      break;

    case RUN:
      if (currentMillis - phaseStartTime >= kRunDurationMs) {
        setMotorOutputs(LOW, LOW);
        Serial.println(F("Motor stopped"));
        phase = STOP;
        phaseStartTime = currentMillis;
      }
      break;

    case STOP:
      if (currentMillis - phaseStartTime >= kStopDurationMs) {
        setMotorOutputs(LOW, HIGH);
        Serial.println(F("Motor running: IN2 direction"));
        phase = RUN;
        phaseStartTime = currentMillis;
      }
      break;
  }
}

#endif // RADIOCORE_HAS_MOTOR_CONTROL
