#include "RelayDriver.h"

void RelayDriver::setup(const Config& cfg) {
  m_cfg = cfg;
  pinMode(m_cfg.pin, OUTPUT);
  setOn(false);
}

void RelayDriver::setOn(bool on) {
  m_isOn = on;
  const uint8_t level = (m_cfg.activeHigh ? (m_isOn ? HIGH : LOW) : (m_isOn ? LOW : HIGH));
  digitalWrite(m_cfg.pin, level);
}

bool RelayDriver::isOn() const {
  return m_isOn;
}
