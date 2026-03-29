#pragma once

#include <Arduino.h>
#include <stdbool.h>

namespace TaskReporting {
  struct Config {
    uint16_t periodMs;
  };

  void setup(const Config& cfg);

  // Call periodically from loop(); prints a report.
  void tick(
    int rawCmdDeg,
    int conditionedCmdDeg,
    int actuatorPosDeg,
    bool limitReached
  );
}
