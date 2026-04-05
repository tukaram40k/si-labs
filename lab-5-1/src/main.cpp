#include <Arduino.h>
#include <stdio.h>
#include "IO.h"
#include "config.h"
#include "tasks/task_actuator_control.h"
#include "tasks/task_conditioning.h"
#include "tasks/task_reporting.h"

void setup()
{
  IO::setup();

  TaskConditioning::setup(Config::CONDITIONING_CFG);
  TaskActuatorControl::setup(Config::ACTUATOR_CFG);
  TaskReporting::setup(Config::REPORTING_CFG);

  printf("Humidity ON-OFF control with hysteresis\n");
  printf("Pins: relay=%u, dht11=%u, up=%u, down=%u\n", Config::RELAY_PIN, Config::DHT11_PIN, Config::BTN_UP_PIN, Config::BTN_DOWN_PIN);
}

void loop()
{
  TaskConditioning::tick();
  const TaskConditioning::State condState = TaskConditioning::getState();

  TaskActuatorControl::commandState(condState.relayRequestOn);
  TaskActuatorControl::tick();

  TaskReporting::tick(
      condState,
      TaskActuatorControl::isOn());
}