#include "Actuator.h"

static Actuator* g_actuator = nullptr;

Actuator::Actuator(const ActuatorConfig& cfg, LedController* statusLed)
  : m_cfg(cfg), m_statusLed(statusLed), m_state(ActuatorState::OFF) {
}

void Actuator::setup() {
  pinMode(m_cfg.relayPin, OUTPUT);
  if (m_statusLed) {
    m_statusLed->setup();
  }
  applyOutputs();
  g_actuator = this;
}

void Actuator::setState(ActuatorState state) {
  m_state = state;
  applyOutputs();
}

void Actuator::setOn(bool on) {
  setState(on ? ActuatorState::ON : ActuatorState::OFF);
}

ActuatorState Actuator::getState() const {
  return m_state;
}

void Actuator::applyOutputs() {
  const bool on = (m_state == ActuatorState::ON);
  const bool relayLevel = m_cfg.activeHigh ? on : !on;
  digitalWrite(m_cfg.relayPin, relayLevel ? HIGH : LOW);

  if (m_statusLed) {
    on ? m_statusLed->turnOn() : m_statusLed->turnOff();
  }
}

ActuatorState actuator_get_state() {
  return g_actuator ? g_actuator->getState() : ActuatorState::OFF;
}
