#include "task_reporting.h"
#include <stdio.h>

namespace
{
  TaskReporting::Config g_cfg{2000};
  uint32_t lastMs = 0;
  uint32_t seq = 0;

  int toTenths(float v)
  {
    const float scaled = v * 10.0f;
    return (scaled >= 0.0f) ? (int)(scaled + 0.5f) : (int)(scaled - 0.5f);
  }

  int absInt(int v)
  {
    return (v < 0) ? -v : v;
  }
}

namespace TaskReporting
{
  void setup(const Config &cfg)
  {
    g_cfg = cfg;
    lastMs = millis();
    seq = 0;

    printf("\n--- Ready ---\n");
  }

  void tick(
      const TaskConditioning::State &cond,
      bool relayOn)
  {
    const uint32_t now = millis();
    if ((uint32_t)(now - lastMs) < g_cfg.periodMs)
    {
      return;
    }
    lastMs = now;

    if (cond.humidityValid)
    {
      const int h10 = toTenths(cond.humidityPct);
      const int lo10 = toTenths(cond.lowerThresholdPct);
      const int hi10 = toTenths(cond.upperThresholdPct);

      printf(
          "[%lu] H=%d.%d %% | SP=%d %% | band=[%d.%d, %d.%d] %% | relay=%s | sensor=OK\n",
          (unsigned long)seq++,
          h10 / 10,
          absInt(h10 % 10),
          cond.setPointPct,
          lo10 / 10,
          absInt(lo10 % 10),
          hi10 / 10,
          absInt(hi10 % 10),
          relayOn ? "ON" : "OFF");
    }
    else
    {
      const int lo10 = toTenths(cond.lowerThresholdPct);
      const int hi10 = toTenths(cond.upperThresholdPct);

      printf(
          "[%lu] H=n/a | SP=%d %% | band=[%d.%d, %d.%d] %% | relay=%s | sensor=ERR\n",
          (unsigned long)seq++,
          cond.setPointPct,
          lo10 / 10,
          absInt(lo10 % 10),
          hi10 / 10,
          absInt(hi10 % 10),
          relayOn ? "ON" : "OFF");
    }
  }
}
