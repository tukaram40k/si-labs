#pragma once

#include <Arduino.h>

namespace TaskActuatorControl
{
  struct Config
  {
    uint8_t servoPin;
    uint16_t periodMs;
    uint16_t minFlipStepPeriodMs;
    uint16_t maxFlipStepPeriodMs;
  };

  void setup(const Config &cfg);

  void commandOutputPct(float outputPct);
  void tick();
  bool isOn();
}
