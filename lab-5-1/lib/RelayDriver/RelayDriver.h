#pragma once

#include <Arduino.h>

class RelayDriver {
public:
  struct Config {
    uint8_t pin;
    bool activeHigh;
  };

  RelayDriver() = default;

  void setup(const Config& cfg);
  void setOn(bool on);
  bool isOn() const;

private:
  Config m_cfg{255, true};
  bool m_isOn = false;
};
