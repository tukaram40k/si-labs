#include <Arduino.h>

#include <stdio.h>

#include "IO.h"

#include "task_actuator_control.h"
#include "task_conditioning.h"
#include "task_reporting.h"

namespace {
  // Raw command as typed by the user (may be out of range).
  volatile int g_lastRawCmdDeg = 0;

  // Non-blocking line reader state.
  char g_lineBuf[16];
  uint8_t g_lineLen = 0;

  static void handleLine(const char* line) {
    int deg = 0;
    // Accept leading/trailing whitespace and an integer.
    if (sscanf(line, "%d", &deg) == 1) {
      g_lastRawCmdDeg = deg;
      TaskConditioning::setRawCommandDeg(deg);
      printf("OK: raw command = %d deg\n", deg);
    } else {
      printf("ERR: please type an integer angle (e.g. 0, 90, 180)\n");
    }
  }

  // Reads from stdin (serial) without blocking.
  static void pollCommandInput() {
    int c = getchar();
    if (c < 0) {
      return; // no char available
    }

    if (c == '\r') {
      return; // ignore CR (we use LF as terminator)
    }

    if (c == '\n') {
      g_lineBuf[g_lineLen] = '\0';
      if (g_lineLen > 0) {
        handleLine(g_lineBuf);
      }
      g_lineLen = 0;
      return;
    }

    // Basic line editing: backspace.
    if (c == 0x08 || c == 0x7F) {
      if (g_lineLen > 0) {
        g_lineLen--;
      }
      return;
    }

    if (g_lineLen < (sizeof(g_lineBuf) - 1)) {
      g_lineBuf[g_lineLen++] = (char)c;
    } else {
      // overflow: reset line
      g_lineLen = 0;
      printf("\nERR: line too long\n");
    }
  }
}

void setup() {
  IO::setup();

  TaskConditioning::setup(TaskConditioning::Config{
    .minDeg = 0,
    .maxDeg = 180,
  .wNew = 1.0f,
  // Minimal ramp: ~400ms for a full 0->180 sweep (18deg per 40ms tick = 10 ticks)
  .rampStepDeg = 18,
  .periodMs = 40,
  });

  TaskActuatorControl::setup(TaskActuatorControl::Config{
    .servoPin = 7,
    .periodMs = 50,
  });

  TaskReporting::setup(TaskReporting::Config{
    .periodMs = 2000,
  });

  // Seed with a safe known command.
  g_lastRawCmdDeg = 0;
  TaskConditioning::setRawCommandDeg(0);
}

void loop() {
  // Input task (serial/stdin)
  pollCommandInput();

  // Conditioning task
  TaskConditioning::tick();
  const auto condState = TaskConditioning::getState();
  const int conditioned = TaskConditioning::getConditionedCommandDeg();

  // Actuator control task
  TaskActuatorControl::commandPositionDeg(conditioned);
  TaskActuatorControl::tick();

  // Reporting task
  TaskReporting::tick(
    g_lastRawCmdDeg,
    conditioned,
    TaskActuatorControl::getPositionDeg(),
    condState.limitReached
  );
}