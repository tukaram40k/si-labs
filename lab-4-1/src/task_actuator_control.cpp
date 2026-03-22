#include "task_actuator_control.h"

#include <stdio.h>
#include <string.h>

#include "../lib/Actuator/Actuator.h"
#include "../lib/RemoteDriver/RemoteDriver.h"

// -------- shared state (input -> conditioning -> actuator) --------

static volatile bool g_hasPendingRequest = false;
static volatile bool g_pendingOn = false;

void control_set_pending_request(bool on) {
  g_pendingOn = on;
  g_hasPendingRequest = true;
}

bool control_consume_pending_request(bool& outOn) {
  if (!g_hasPendingRequest) {
    return false;
  }
  outOn = g_pendingOn;
  g_hasPendingRequest = false;
  return true;
}

// Exposed for reporting/conditioning.
volatile uint32_t g_alert_unknown_cmd = 0;
volatile uint32_t g_alert_unknown_ir = 0;
volatile uint32_t g_rx_serial_cmd = 0;
volatile uint32_t g_rx_remote_cmd = 0;
volatile uint32_t g_last_unknown_ir_code = 0;

// -------- remote receiver --------

static RemoteDriver* g_remote = nullptr;

void task_actuator_control_bind_remote(RemoteDriver* remote) {
  g_remote = remote;
}

static void poll_remote() {
  if (!g_remote) {
    return;
  }

  bool on = false;
  bool known = false;
  uint32_t raw = 0;
  if (!g_remote->poll(on, known, raw)) {
    return;
  }

  if (known) {
    control_set_pending_request(on);
    g_rx_remote_cmd++;
  } else {
    g_alert_unknown_ir++;
    g_last_unknown_ir_code = raw;
  }
}

void task_actuator_control_setup() {
  // remote-only control
}

void task_actuator_control_tick() {
  poll_remote();
}
