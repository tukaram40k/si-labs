#include "LedDriver.h"

void LedDriver::setup(const Config& cfg) {
  m_cfg = cfg;
  pinMode(m_cfg.pin, OUTPUT);
  m_isOn = false;
  digitalWrite(m_cfg.pin, m_cfg.activeHigh ? LOW : HIGH);
}

void LedDriver::set(bool on) {
  if (m_cfg.pin == 255) {
    return;
  }

  if (m_isOn == on) {
    return;
  }
  m_isOn = on;

  const bool levelHigh = m_cfg.activeHigh ? on : !on;
  digitalWrite(m_cfg.pin, levelHigh ? HIGH : LOW);
}
