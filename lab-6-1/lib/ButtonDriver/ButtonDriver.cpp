#include "ButtonDriver.h"

namespace {
  bool isPressedRaw(uint8_t pin, bool activeLow) {
    const int level = digitalRead(pin);
    return activeLow ? (level == LOW) : (level == HIGH);
  }
}

void ButtonDriver::setup(const Config& cfg) {
  m_cfg = cfg;
  pinMode(m_cfg.pin, m_cfg.activeLow ? INPUT_PULLUP : INPUT);

  m_lastRawPressed = isPressedRaw(m_cfg.pin, m_cfg.activeLow);
  m_stablePressed = m_lastRawPressed;
  m_lastRawChangeMs = millis();
}

bool ButtonDriver::pollPressEvent() {
  const uint32_t now = millis();
  const bool rawPressed = isPressedRaw(m_cfg.pin, m_cfg.activeLow);

  if (rawPressed != m_lastRawPressed) {
    m_lastRawPressed = rawPressed;
    m_lastRawChangeMs = now;
  }

  if ((uint32_t)(now - m_lastRawChangeMs) < m_cfg.debounceMs) {
    return false;
  }

  if (m_stablePressed != m_lastRawPressed) {
    m_stablePressed = m_lastRawPressed;
    if (m_stablePressed) {
      return true;
    }
  }

  return false;
}
