#include "task_conditioning.h"

namespace
{
  TaskConditioning::Config g_cfg{0, 180, 0.35f, 8, 5, 50};
  TaskConditioning::State g_state{};

  int g_rawLatched = 0;
  int h0 = 0, h1 = 0, h2 = 0;

  float avg = 0.0f;

  uint32_t lastMs = 0;

  static int clampInt(int v, int lo, int hi)
  {
    if (v < lo)
      return lo;
    if (v > hi)
      return hi;
    return v;
  }

  static int median3(int a, int b, int c)
  {
    if (a > b)
    {
      int t = a;
      a = b;
      b = t;
    }
    if (b > c)
    {
      int t = b;
      b = c;
      c = t;
    }
    if (a > b)
    {
      int t = a;
      a = b;
      b = t;
    }
    return b;
  }
}

namespace TaskConditioning
{
  void setup(const Config &cfg)
  {
    g_cfg = cfg;

    g_rawLatched = 0;
    h0 = h1 = h2 = 0;
    avg = 0.0f;

    g_state = {};
    g_state.rawCmdDeg = 0;
    g_state.saturatedDeg = 0;
    g_state.medianDeg = 0;
    g_state.averagedDeg = 0;
    g_state.rampedDeg = 0;
    g_state.limitReached = false;

    lastMs = millis();
  }

  void setRawCommandDeg(int rawDeg)
  {
    g_rawLatched = rawDeg;
  }

  void tick()
  {
    const uint32_t now = millis();
    if (g_cfg.periodMs > 0 && (uint32_t)(now - lastMs) < g_cfg.periodMs)
    {
      return;
    }
    lastMs = now;

    g_state.rawCmdDeg = g_rawLatched;

    const int sat = clampInt(g_state.rawCmdDeg, g_cfg.minDeg, g_cfg.maxDeg);
    g_state.saturatedDeg = sat;
    g_state.limitReached = (sat != g_state.rawCmdDeg);

    const int bypass = (g_cfg.stepBypassDeg <= 0) ? 9999 : g_cfg.stepBypassDeg;
    const int deltaFromPrev = (sat >= g_state.averagedDeg) ? (sat - g_state.averagedDeg) : (g_state.averagedDeg - sat);

    int preRampTarget = sat;
    if (deltaFromPrev < bypass)
    {
      h0 = h1;
      h1 = h2;
      h2 = sat;
      const int med = median3(h0, h1, h2);
      g_state.medianDeg = med;

      const float w = (g_cfg.wNew < 0.0f) ? 0.0f : ((g_cfg.wNew > 1.0f) ? 1.0f : g_cfg.wNew);
      avg = (w * (float)med) + ((1.0f - w) * avg);
      const int avgInt = (int)(avg + 0.5f);
      g_state.averagedDeg = avgInt;
      preRampTarget = avgInt;
    }
    else
    {
      g_state.medianDeg = sat;
      g_state.averagedDeg = sat;
      avg = (float)sat;
      preRampTarget = sat;
      h0 = h1 = h2 = sat;
    }

    const int target = clampInt(preRampTarget, g_cfg.minDeg, g_cfg.maxDeg);
    int ramped = g_state.rampedDeg;

    const int step = (g_cfg.rampStepDeg <= 0) ? 180 : g_cfg.rampStepDeg;
    if (target > ramped)
    {
      ramped += step;
      if (ramped > target)
        ramped = target;
    }
    else if (target < ramped)
    {
      ramped -= step;
      if (ramped < target)
        ramped = target;
    }
    ramped = clampInt(ramped, g_cfg.minDeg, g_cfg.maxDeg);
    g_state.rampedDeg = ramped;
  }

  int getConditionedCommandDeg()
  {
    return g_state.rampedDeg;
  }

  State getState()
  {
    return g_state;
  }
}
