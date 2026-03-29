#include <Arduino.h>
#include <stdio.h>
#include "IO.h"
#include "RemoteDriver.h"

#include "task_actuator_control.h"
#include "task_conditioning.h"
#include "task_reporting.h"

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
  constexpr uint8_t kIrPin = 2;

  // Codes from src/IRCodes.cpp
  constexpr uint32_t kCodeUp = 0xD92655AA;
  constexpr uint32_t kCodeRight = 0xDB2455AA;
  constexpr uint32_t kCodeDown = 0xD82755AA;

  // RemoteDriver is ON/OFF oriented; we use it for raw decoding and map codes ourselves.
  RemoteDriver g_remote(RemoteDriver::Config{
      .irReceivePin = kIrPin,
      .codeUp = kCodeUp,
      .codeRight = kCodeRight,
      .codeDown = kCodeDown,
      .codeLeft = 0xDA2555AA,
  });

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

  TaskConditioning::setup(TaskConditioning::Config{
      .minDeg = 0,
      .maxDeg = 180,
      // Minimal median + WMA: only smooth small fluctuations.
      .wNew = 0.6f,
      // If user jumps a lot (e.g. 10 -> 170), bypass smoothing for responsiveness.
      .stepBypassDeg = 25,
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
      g_lastRawCmdDeg,
      conditioned,
      TaskActuatorControl::getPositionDeg(),
      condState.limitReached);
}