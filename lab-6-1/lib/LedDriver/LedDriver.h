#pragma once

#include <Arduino.h>

class LedDriver {
public:
  struct Config {
    uint8_t pin;
    bool activeHigh;
  };

  LedDriver() = default;

  void setup(const Config& cfg);
  void set(bool on);

private:
  Config m_cfg{255, true};
  bool m_isOn = false;
};
