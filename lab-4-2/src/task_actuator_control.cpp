#include "task_actuator_control.h"

#include <Servo.h>

namespace {
  Servo g_servo;
  TaskActuatorControl::Config g_cfg{7, 50};

  int g_commandDeg = 0;
  int g_appliedDeg = 0;

  uint32_t g_lastUpdateMs = 0;
}

namespace TaskActuatorControl {
  void setup(const Config& cfg) {
    g_cfg = cfg;
    g_servo.attach(g_cfg.servoPin);

    g_commandDeg = 0;
    g_appliedDeg = 0;
    g_servo.write(g_appliedDeg);

    g_lastUpdateMs = millis();
  }

  void commandPositionDeg(int positionDeg) {
    g_commandDeg = positionDeg;
  }

  void tick() {
    const uint32_t now = millis();
    if ((uint32_t)(now - g_lastUpdateMs) < g_cfg.periodMs) {
      return;
    }
    g_lastUpdateMs = now;

    if (g_appliedDeg != g_commandDeg) {
      g_appliedDeg = g_commandDeg;
      g_servo.write(g_appliedDeg);
    }
  }

  int getPositionDeg() {
    return g_appliedDeg;
  }
}
