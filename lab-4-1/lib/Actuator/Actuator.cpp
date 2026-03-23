#include "Actuator.h"

static Actuator* g_actuator = nullptr;

Actuator::Actuator(const ActuatorConfig& cfg, void*)
  : m_cfg(cfg), m_state(ActuatorState::OFF) {
}

void Actuator::setup() {
  pinMode(m_cfg.relayPin, OUTPUT);
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
}

ActuatorState actuator_get_state() {
  return g_actuator ? g_actuator->getState() : ActuatorState::OFF;
}
