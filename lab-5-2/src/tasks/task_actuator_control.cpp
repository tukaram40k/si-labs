#include "task_actuator_control.h"

#include "ServoDriver.h"

namespace
{
  TaskActuatorControl::Config g_cfg{7, 50};
  ServoDriver g_servo;

  bool g_commandOn = false;
  bool g_appliedOn = false;

  uint32_t g_lastUpdateMs = 0;
}

namespace TaskActuatorControl
{
  void setup(const Config &cfg)
  {
    g_cfg = cfg;
    g_servo.setup(ServoDriver::Config{.pin = g_cfg.servoPin});

    g_commandOn = false;
    g_appliedOn = false;
    g_servo.stop_flipping();

    g_lastUpdateMs = millis();
  }

  void commandState(bool actuatorOn)
  {
    g_commandOn = actuatorOn;
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

    if (g_appliedOn != g_commandOn)
    {
      g_appliedOn = g_commandOn;

      if (g_appliedOn)
      {
        g_servo.start_flipping();
      }
      else
      {
        g_servo.stop_flipping();
      }
    }
  }

  bool isOn()
  {
    return g_appliedOn;
  }
}
