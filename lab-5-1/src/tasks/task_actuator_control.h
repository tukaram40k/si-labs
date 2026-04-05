#pragma once

#include <Arduino.h>

namespace TaskActuatorControl
{
  struct Config
  {
    uint8_t relayPin;
    bool relayActiveHigh;
    uint16_t periodMs;
  };

  void setup(const Config &cfg);

  void commandState(bool relayOn);
  void tick();
  bool isOn();
}
