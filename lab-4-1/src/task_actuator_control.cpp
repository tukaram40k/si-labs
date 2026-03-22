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
volatile uint32_t g_rx_serial_cmd = 0;
volatile uint32_t g_rx_remote_cmd = 0;

// -------- stdio command receiver (non-blocking) --------

static char g_lineBuf[32];
static uint8_t g_lineLen = 0;

static void handle_line(const char* line) {
  // Trim leading/trailing spaces.
  while (*line == ' ' || *line == '\t') {
    line++;
  }
  size_t len = strlen(line);
  while (len > 0 && (line[len - 1] == ' ' || line[len - 1] == '\t')) {
    len--;
  }
  if (len == 0) {
    return;
  }

  char tmp[32];
  if (len >= sizeof(tmp)) {
    len = sizeof(tmp) - 1;
  }
  memcpy(tmp, line, len);
  tmp[len] = '\0';

  // Accept: ON / OFF (case-insensitive), also 1/0.
  if (strcasecmp(tmp, "ON") == 0 || strcmp(tmp, "1") == 0) {
    control_set_pending_request(true);
  g_rx_serial_cmd++;
    return;
  }
  if (strcasecmp(tmp, "OFF") == 0 || strcmp(tmp, "0") == 0) {
    control_set_pending_request(false);
  g_rx_serial_cmd++;
    return;
  }

  g_alert_unknown_cmd++;
}

static void poll_stdio_line() {
  // IO module redirects stdin to Serial; do a non-blocking poll.
  while (Serial.available() > 0) {
    const char c = (char)Serial.read();

    if (c == '\r') {
      continue;
    }

    if (c == '\n') {
      g_lineBuf[g_lineLen] = '\0';
      if (g_lineLen > 0) {
        handle_line(g_lineBuf);
      }
      g_lineLen = 0;
      continue;
    }

    // Simple saturation on input length.
    if (g_lineLen < (sizeof(g_lineBuf) - 1)) {
      g_lineBuf[g_lineLen++] = c;
    } else {
      // buffer full, drop further characters until newline
    }
  }
}

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
  if (g_remote->poll(on)) {
    control_set_pending_request(on);
  g_rx_remote_cmd++;
  }
}

void task_actuator_control_setup() {
  g_lineLen = 0;
}

void task_actuator_control_tick() {
  poll_stdio_line();
  poll_remote();
}
