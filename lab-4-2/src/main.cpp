#include <Arduino.h>
#include <stdio.h>
#include "IO.h"
#include "RemoteDriver.h"
#include "config.h"
#include "tasks/task_actuator_control.h"
#include "tasks/task_conditioning.h"
#include "tasks/task_reporting.h"

namespace
{
  // Raw command as typed by the user (may be out of range).
  volatile int g_lastRawCmdDeg = 0;

  // Non-blocking line reader state.
  char g_lineBuf[16];
  uint8_t g_lineLen = 0;

  static void handleLine(const char *line)
  {
    int deg = 0;
    // Accept leading/trailing whitespace and an integer.
    if (sscanf(line, "%d", &deg) == 1)
    {
      g_lastRawCmdDeg = deg;
      TaskConditioning::setRawCommandDeg(deg);
      printf("OK: raw command = %d deg\n", deg);
    }
    else
    {
      printf("ERR: please type an integer angle (e.g. 0, 90, 180)\n");
    }
  }

  // Reads from stdin (serial) without blocking.
  static void pollCommandInput()
  {
    int c = getchar();
    if (c < 0)
    {
      return; // no char available
    }

    if (c == '\r')
    {
      return; // ignore CR (we use LF as terminator)
    }

    if (c == '\n')
    {
      g_lineBuf[g_lineLen] = '\0';
      if (g_lineLen > 0)
      {
        handleLine(g_lineBuf);
      }
      g_lineLen = 0;
      return;
    }

    // Basic line editing: backspace.
    if (c == 0x08 || c == 0x7F)
    {
      if (g_lineLen > 0)
      {
        g_lineLen--;
      }
      return;
    }

    if (g_lineLen < (sizeof(g_lineBuf) - 1))
    {
      g_lineBuf[g_lineLen++] = (char)c;
    }
    else
    {
      // overflow: reset line
      g_lineLen = 0;
      printf("\nERR: line too long\n");
    }
  }

  // --- IR remote handling ---
  RemoteDriver g_remote(Config::REMOTE_CFG);

  static void pollRemote()
  {
    RemoteDriver::Button btn = RemoteDriver::Button::Unknown;
    uint32_t raw = 0;
    if (!g_remote.poll(btn, raw))
    {
      return;
    }

    int deg = -1;
    if (btn == RemoteDriver::Button::Up)
      deg = 0;
    else if (btn == RemoteDriver::Button::Right)
      deg = 90;
    else if (btn == RemoteDriver::Button::Down)
      deg = 180;

    if (deg >= 0)
    {
      g_lastRawCmdDeg = deg;
      TaskConditioning::setRawCommandDeg(deg);
      printf("IR: raw command = %d deg\n", deg);
    }
    else
    {
      // Unknown/unused code; keep quiet to avoid log spam.
      (void)raw;
    }
  }
}

void setup()
{
  IO::setup();

  g_remote.setup();

  TaskConditioning::setup(Config::CONDITIONING_CFG);
  TaskActuatorControl::setup(Config::ACTUATOR_CFG);
  TaskReporting::setup(Config::REPORTING_CFG);

  // Seed with a safe known command.
  g_lastRawCmdDeg = 0;
  TaskConditioning::setRawCommandDeg(0);
}

void loop()
{
  // Input task (serial/stdin)
  pollCommandInput();

  // IR remote task
  pollRemote();

  // Conditioning task
  TaskConditioning::tick();
  const auto condState = TaskConditioning::getState();
  const int conditioned = TaskConditioning::getConditionedCommandDeg();

  // Actuator control task
  TaskActuatorControl::commandPositionDeg(conditioned);
  TaskActuatorControl::tick();

  // Reporting task
  TaskReporting::tick(
      condState,
      TaskActuatorControl::getPositionDeg(),
      condState.limitReached);
}