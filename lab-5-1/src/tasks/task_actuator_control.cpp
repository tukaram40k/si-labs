#include "task_actuator_control.h"

#include "RelayDriver.h"

namespace
{
  TaskActuatorControl::Config g_cfg{7, true, 50};
  RelayDriver g_driver;

  bool g_commandOn = false;
  bool g_appliedOn = false;

  uint32_t g_lastUpdateMs = 0;
}

namespace TaskActuatorControl
{
  void setup(const Config &cfg)
  {
    g_cfg = cfg;
    g_driver.setup(RelayDriver::Config{.pin = g_cfg.relayPin, .activeHigh = g_cfg.relayActiveHigh});

    g_commandOn = false;
    g_appliedOn = false;
    g_driver.setOn(g_appliedOn);

    g_lastUpdateMs = millis();
  }

  void commandState(bool relayOn)
  {
    g_commandOn = relayOn;
  }

  void tick()
  {
    const uint32_t now = millis();
    if ((uint32_t)(now - g_lastUpdateMs) < g_cfg.periodMs)
    {
      return;
    }
    g_lastUpdateMs = now;

    if (g_appliedOn != g_commandOn)
    {
      g_appliedOn = g_commandOn;
      g_driver.setOn(g_appliedOn);
    }
  }

  bool isOn()
  {
    return g_appliedOn;
  }
}
