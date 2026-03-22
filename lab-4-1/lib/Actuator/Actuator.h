#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <Arduino.h>
#include "../LedDriver/LedController.h"

enum class ActuatorState : uint8_t {
  OFF = 0,
  ON = 1,
};

struct ActuatorConfig {
  uint8_t relayPin;
  // Relay polarity: true means HIGH energizes relay (typical).
  bool activeHigh;
};

// Binary actuator (relay-driven LED). Owns the relay output and a status LED.
class Actuator {
public:
  Actuator(const ActuatorConfig& cfg, LedController* statusLed);

  void setup();

  void setState(ActuatorState state);
  void setOn(bool on);

  ActuatorState getState() const;

private:
  void applyOutputs();

  ActuatorConfig m_cfg;
  LedController* m_statusLed;
  ActuatorState m_state;
};

// C-style interface as requested by the lab text.
ActuatorState actuator_get_state();

#endif
