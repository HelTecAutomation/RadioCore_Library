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
