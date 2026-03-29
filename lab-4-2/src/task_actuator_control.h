#pragma once

#include <Arduino.h>

namespace TaskActuatorControl {
  struct Config {
    uint8_t servoPin;
    uint16_t periodMs; // control update period
  };

  void setup(const Config& cfg);

  // Set desired actuator position (degrees). The value is assumed already conditioned.
  void commandPositionDeg(int positionDeg);

  // Call periodically from loop(); performs rate-limited servo updates.
  void tick();

  int getPositionDeg();
}
