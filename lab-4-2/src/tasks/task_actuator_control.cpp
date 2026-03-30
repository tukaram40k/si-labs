#include "task_actuator_control.h"
#include "ServoDriver.h"

namespace
{
  TaskActuatorControl::Config g_cfg{7, 50};
  ServoDriver g_driver;

  int g_commandDeg = 0;
  int g_appliedDeg = 0;

  uint32_t g_lastUpdateMs = 0;
}

namespace TaskActuatorControl
{
  void setup(const Config &cfg)
  {
    g_cfg = cfg;
    g_driver.setup(ServoDriver::Config{.pin = g_cfg.servoPin});

    g_commandDeg = 0;
    g_appliedDeg = 0;
    g_driver.writeDeg(g_appliedDeg);

    g_lastUpdateMs = millis();
  }

  void commandPositionDeg(int positionDeg)
  {
    g_commandDeg = positionDeg;
  }

  void tick()
  {
    const uint32_t now = millis();
    if ((uint32_t)(now - g_lastUpdateMs) < g_cfg.periodMs)
    {
      return;
    }
    g_lastUpdateMs = now;

    if (g_appliedDeg != g_commandDeg)
    {
      g_appliedDeg = g_commandDeg;
      g_driver.writeDeg(g_appliedDeg);
    }
  }

  int getPositionDeg()
  {
    return g_appliedDeg;
  }
}
