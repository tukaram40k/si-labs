#pragma once

#include <Arduino.h>

class ButtonDriver {
public:
  struct Config {
    uint8_t pin;
    bool activeLow;
    uint16_t debounceMs;
  };

  ButtonDriver() = default;

  void setup(const Config& cfg);

  // Returns true once per stable press edge.
  bool pollPressEvent();

private:
  Config m_cfg{255, true, 25};
  bool m_lastRawPressed = false;
  bool m_stablePressed = false;
  uint32_t m_lastRawChangeMs = 0;
};
