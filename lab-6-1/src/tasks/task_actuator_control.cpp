#include "task_actuator_control.h"

#include "ServoDriver.h"

namespace
{
  TaskActuatorControl::Config g_cfg{7, 50, 10, 80};
  ServoDriver g_servo;

  float g_commandOutputPct = 0.0f;
  bool g_appliedOn = false;

  uint32_t g_lastUpdateMs = 0;

  float clampFloat(float v, float lo, float hi)
  {
    if (v < lo)
      return lo;
    if (v > hi)
      return hi;
    return v;
  }
}

namespace TaskActuatorControl
{
  void setup(const Config &cfg)
  {
    g_cfg = cfg;
    g_servo.setup(ServoDriver::Config{.pin = g_cfg.servoPin});

    g_commandOutputPct = 0.0f;
    g_appliedOn = false;
    g_servo.stop_flipping();

    g_lastUpdateMs = millis();
  }

  void commandOutputPct(float outputPct)
  {
    g_commandOutputPct = outputPct;
  }

  void tick()
  {
    g_servo.tick();

    const uint32_t now = millis();
    if ((uint32_t)(now - g_lastUpdateMs) < g_cfg.periodMs)
    {
      return;
    }
    g_lastUpdateMs = now;

    const float outputPct = clampFloat(g_commandOutputPct, 0.0f, 100.0f);
    const bool shouldOn = (outputPct > 0.0f);
    if (!shouldOn)
    {
      if (g_appliedOn)
      {
        g_appliedOn = false;
        g_servo.stop_flipping();
      }
      return;
    }

    if (!g_appliedOn)
    {
      g_appliedOn = true;
      g_servo.start_flipping();
    }

    uint16_t minStepMs = g_cfg.minFlipStepPeriodMs;
    uint16_t maxStepMs = g_cfg.maxFlipStepPeriodMs;
    if (minStepMs == 0)
    {
      minStepMs = 1;
    }
    if (maxStepMs < minStepMs)
    {
      maxStepMs = minStepMs;
    }

    const float range = (float)(maxStepMs - minStepMs);
    const float maxSpeedPct = 50.0f;
    float scaledPct = outputPct / maxSpeedPct;
    if (scaledPct > 1.0f)
    {
      scaledPct = 1.0f;
    }

    const uint16_t stepMs = (uint16_t)(maxStepMs - (scaledPct * range) + 0.5f);
    g_servo.setFlipStepPeriodMs(stepMs);
  }

  bool isOn()
  {
    return g_appliedOn;
  }
}
