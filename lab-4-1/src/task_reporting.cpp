#include "task_reporting.h"

#include <stdio.h>

#include "task_signal_conditioning.h"

extern volatile uint32_t g_alert_unknown_cmd;
extern volatile uint32_t g_rx_serial_cmd;
extern volatile uint32_t g_rx_remote_cmd;

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
  const bool pending = conditioning_has_unapplied_request();

  const uint32_t unknownCmd = g_alert_unknown_cmd;
  const uint32_t rxSerial = g_rx_serial_cmd;
  const uint32_t rxRemote = g_rx_remote_cmd;
  const bool newUnknown = (unknownCmd != g_lastUnknownCmd);
  g_lastUnknownCmd = unknownCmd;

  printf("{\"t_ms\":%lu,\"state\":\"%s\",\"pending\":%s,\"rx\":{\"serial\":%lu,\"remote\":%lu},\"alerts\":{\"unknown_cmd\":%lu%s}}\n",
         (unsigned long)millis(),
         state_to_str(st),
         pending ? "true" : "false",
         (unsigned long)rxSerial,
         (unsigned long)rxRemote,
         (unsigned long)unknownCmd,
         newUnknown ? ",\"note\":\"unknown command received\"" : "");
}
