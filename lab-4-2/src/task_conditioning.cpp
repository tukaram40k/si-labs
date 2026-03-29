#include "task_conditioning.h"

namespace {
  TaskConditioning::Config g_cfg{0, 180, 0.35f, 5, 50};
  TaskConditioning::State g_state{};

  int g_rawLatched = 0;

  // median history (3 samples)
  int h0 = 0, h1 = 0, h2 = 0;

  float avg = 0.0f;

  uint32_t lastMs = 0;

  static int clampInt(int v, int lo, int hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
  }

  static int median3(int a, int b, int c) {
    if (a > b) { int t=a; a=b; b=t; }
    if (b > c) { int t=b; b=c; c=t; }
    if (a > b) { int t=a; a=b; b=t; }
    return b;
  }
}

namespace TaskConditioning {
  void setup(const Config& cfg) {
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

  void setRawCommandDeg(int rawDeg) {
    g_rawLatched = rawDeg;
  }

  void tick() {
    const uint32_t now = millis();
    if (g_cfg.periodMs > 0 && (uint32_t)(now - lastMs) < g_cfg.periodMs) {
      return;
    }
    lastMs = now;

    g_state.rawCmdDeg = g_rawLatched;

    const int sat = clampInt(g_state.rawCmdDeg, g_cfg.minDeg, g_cfg.maxDeg);
    g_state.saturatedDeg = sat;
    g_state.limitReached = (sat != g_state.rawCmdDeg);

    // Minimal conditioning compromise:
    // - saturation (limits)
    // - ramping only (smooth motion)
    // Keep median/averaging as pass-through for reporting compatibility.
    g_state.medianDeg = sat;
    g_state.averagedDeg = sat;

    const int target = sat;
    int ramped = g_state.rampedDeg;

    const int step = (g_cfg.rampStepDeg <= 0) ? 180 : g_cfg.rampStepDeg;
    if (target > ramped) {
      ramped += step;
      if (ramped > target) ramped = target;
    } else if (target < ramped) {
      ramped -= step;
      if (ramped < target) ramped = target;
    }
    ramped = clampInt(ramped, g_cfg.minDeg, g_cfg.maxDeg);
    g_state.rampedDeg = ramped;
  }

  int getConditionedCommandDeg() {
    return g_state.rampedDeg;
  }

  State getState() {
    return g_state;
  }
}
