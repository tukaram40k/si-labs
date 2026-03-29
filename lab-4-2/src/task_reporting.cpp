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

    printf("\n--- Servo control ready ---\n");
    printf("Type an angle (0..180) and press Enter.\n\n");
  }

  void tick(
      int rawCmdDeg,
      int conditionedCmdDeg,
      int actuatorPosDeg,
      bool limitReached)
  {
    const uint32_t now = millis();
    if ((uint32_t)(now - lastMs) < g_cfg.periodMs)
    {
      return;
    }
    lastMs = now;

    printf("[%lu] raw=%d deg | conditioned=%d deg | actuator=%d deg | limit=%s\n",
           (unsigned long)seq++,
           rawCmdDeg,
           conditionedCmdDeg,
           actuatorPosDeg,
           limitReached ? "YES" : "NO");
  }
}
