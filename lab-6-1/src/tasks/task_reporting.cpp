#include "task_reporting.h"
#include <stdio.h>

namespace
{
  TaskReporting::Config g_cfg{2000};
  uint32_t lastMs = 0;

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
    printf("\n--- Ready ---\n");
  }

  void tick(
      const TaskConditioning::State &cond,
      bool actuatorOn)
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
      const int out10 = toTenths(cond.controlOutputPct);
      const int duty10 = toTenths(cond.controlDutyPct);

      printf(
          ">SetPoint:%d,Value:%d.%d,Output:%d.%d\n",
          cond.setPointPct,
          h10 / 10,
          absInt(h10 % 10),
          out10 / 10,
          absInt(out10 % 10));
      printf(
          "# H=%d.%d %% | SP=%d %% | OUT=%d.%d %% | Duty=%d.%d %%\n",
          h10 / 10,
          absInt(h10 % 10),
          cond.setPointPct,
          out10 / 10,
          absInt(out10 % 10),
          duty10 / 10,
          absInt(duty10 % 10));
    }
    else
    {
      printf(
          ">SetPoint:%d,Value:0.0,Output:0.0\n",
          cond.setPointPct);
      printf(
          "# H=n/a | SP=%d %% | OUT=0.0 %% | Duty=0.0 %%\n",
          cond.setPointPct);
    }
  }
}
