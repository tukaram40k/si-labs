#pragma once

#include <Arduino.h>
#include <stdbool.h>

#include "task_conditioning.h"

namespace TaskReporting
{
  struct Config
  {
    uint16_t periodMs;
  };

  void setup(const Config &cfg);

  // Call periodically from loop(); prints a report.
  void tick(
      const TaskConditioning::State &cond,
      int actuatorPosDeg,
      bool limitReached);
}
