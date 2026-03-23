#ifndef ACTUATOR_H
#define ACTUATOR_H

#include <Arduino.h>

enum class ActuatorState : uint8_t {
  OFF = 0,
  ON = 1,
};

struct ActuatorConfig {
  uint8_t relayPin;
  // Relay polarity: true means HIGH energizes relay (typical).
  bool activeHigh;
};

// Binary actuator (relay). Owns only the relay output.
class Actuator {
public:
  Actuator(const ActuatorConfig& cfg, void* /*unused*/);

  void setup();

  void setState(ActuatorState state);
  void setOn(bool on);

  ActuatorState getState() const;

private:
  void applyOutputs();

  ActuatorConfig m_cfg;
  ActuatorState m_state;
};

ActuatorState actuator_get_state();

#endif
