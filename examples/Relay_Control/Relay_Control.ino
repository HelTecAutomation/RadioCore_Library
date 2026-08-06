/*
 * Connect an active-high relay module input to RC32 GPIO2, RCC6 GPIO1, or RC52
 * P1.15. Connect module logic ground to board ground and use an appropriate
 * driver and supply; never drive a relay coil directly from GPIO.
 *
 * RC32 GPIO2 overlaps the gas-sensor ADC. RCC6 GPIO1 overlaps Sensor I2C SCL
 * and the NV3001B backlight. The relay toggles every 3 s by default.
 */
#ifndef RELAY_TOGGLE_INTERVAL_MS
#define RELAY_TOGGLE_INTERVAL_MS 3000UL
#endif

#include <Arduino.h>
#include <RadioCore.h>

#if !RADIOCORE_HAS_RELAY_CONTROL
#error "Relay_Control requires a RadioCore relay control board configuration."
#else

namespace {

bool relayActive = false;

uint8_t relayLevel(bool active)
{
  if (active) {
    return RADIOCORE_RELAY_ACTIVE_LEVEL;
  }

  return RADIOCORE_RELAY_ACTIVE_LEVEL == HIGH ? LOW : HIGH;
}

void setRelayActive(bool active)
{
  relayActive = active;
  digitalWrite(RADIOCORE_RELAY_CONTROL_PIN, relayLevel(active));
  Serial.println(active ? F("Relay activated") : F("Relay released"));
}

} // namespace

void setup()
{
  Serial.begin(115200);

  digitalWrite(RADIOCORE_RELAY_CONTROL_PIN, relayLevel(false));
  pinMode(RADIOCORE_RELAY_CONTROL_PIN, OUTPUT);
  setRelayActive(false);
}

void loop()
{
  delay(RELAY_TOGGLE_INTERVAL_MS);
  setRelayActive(!relayActive);
}

#endif // RADIOCORE_HAS_RELAY_CONTROL
