#pragma once

#include <Arduino.h>

namespace TaskConditioning
{
  struct Config
  {
    int minDeg;
    int maxDeg;

    // Median filter window size: fixed 3-sample median.

    // Weighted average: y = (wNew*x + (1-wNew)*yPrev)
    // Set to 1.0f to effectively disable averaging.
    float wNew;

    // If the target changes by >= this amount, bypass median/avg and jump to it.
    // Keeps the system responsive while still smoothing small noise.
    int stepBypassDeg;

    // Ramping (deg per tick)
    int rampStepDeg;

    uint16_t periodMs;
  };

  struct State
  {
    int rawCmdDeg;
    int saturatedDeg;
    int medianDeg;
    int averagedDeg;
    int rampedDeg;

    bool limitReached;
  };

  void setup(const Config &cfg);

  // Provide a new raw command. This is latched until processed.
  void setRawCommandDeg(int rawDeg);

  // Call periodically from loop(); updates conditioning pipeline.
  void tick();

  // Latest conditioned command.
  int getConditionedCommandDeg();

  State getState();
}
