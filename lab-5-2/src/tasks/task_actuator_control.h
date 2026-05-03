#pragma once

#include <Arduino.h>

namespace TaskActuatorControl
{
  struct Config
  {
    uint8_t servoPin;
    uint16_t periodMs;
  };

  void setup(const Config &cfg);

  void commandState(bool actuatorOn);
  void tick();
  bool isOn();
}
