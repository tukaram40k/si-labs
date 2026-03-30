#include "task_reporting.h"
#include <stdio.h>

namespace
{
  TaskReporting::Config g_cfg{2000};
  uint32_t lastMs = 0;
  uint32_t seq = 0;
}

namespace TaskReporting
{
  void setup(const Config &cfg)
  {
    g_cfg = cfg;
    lastMs = millis();
    seq = 0;

    printf("\n--- Ready ---\n\n");
  }

  void tick(
      const TaskConditioning::State &cond,
      int actuatorPosDeg,
      bool limitReached)
  {
    const uint32_t now = millis();
    if ((uint32_t)(now - lastMs) < g_cfg.periodMs)
    {
      return;
    }
    lastMs = now;

    printf(
        "[%lu] raw=%d | sat=%d | med=%d | wma=%d | ramp=%d | act=%d | limit=%s\n",
        (unsigned long)seq++,
        cond.rawCmdDeg,
        cond.saturatedDeg,
        cond.medianDeg,
        cond.averagedDeg,
        cond.rampedDeg,
        actuatorPosDeg,
        limitReached ? "YES" : "NO");
  }
}
