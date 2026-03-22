#include "task_reporting.h"

#include <stdio.h>

extern volatile uint32_t g_alert_unknown_cmd;
extern volatile uint32_t g_alert_unknown_ir;
extern volatile uint32_t g_rx_serial_cmd;
extern volatile uint32_t g_rx_remote_cmd;
extern volatile uint32_t g_last_unknown_ir_code;

static Actuator* g_actuator = nullptr;
static uint32_t g_lastUnknownCmd = 0;

void task_reporting_setup(Actuator* actuator) {
  g_actuator = actuator;
  g_lastUnknownCmd = 0;
}

static const char* state_to_str(ActuatorState s) {
  return (s == ActuatorState::ON) ? "ON" : "OFF";
}

void task_reporting_tick() {
  if (!g_actuator) {
    return;
  }

  const ActuatorState st = g_actuator->getState();
  const uint32_t unknownIr = g_alert_unknown_ir;
  const uint32_t rxRemote = g_rx_remote_cmd;

  // report
  printf(
    "t=%6lums | state=%-3s | remote=%lu | unknown_command=%lu\n",
    (unsigned long)millis(),
    state_to_str(st),
    (unsigned long)rxRemote,
    (unsigned long)unknownIr
  );
}
