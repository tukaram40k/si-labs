#include <Arduino.h>
#include <stdio.h>
#include "ButtonDriver.h"
#include "IO.h"
#include "LedDriver.h"
#include "config.h"

#define LED_OFF_STATE 0
#define LED_ON_STATE 1

struct State
{
  unsigned long Out;
  unsigned long Time;
  unsigned long Next[2];
};
typedef const struct State STyp;

STyp FSM[2] = {
    {0, 1, {LED_OFF_STATE, LED_ON_STATE}},
    {1, 1, {LED_ON_STATE, LED_OFF_STATE}}};

int FSM_State = LED_OFF_STATE;

namespace
{
  ButtonDriver g_button;
  LedDriver g_led;
  int g_lastState = LED_OFF_STATE;
}

void setup()
{
  IO::setup();

  g_button.setup(ButtonDriver::Config{
      .pin = Config::BUTTON_PIN,
      .activeLow = Config::BUTTON_ACTIVE_LOW,
      .debounceMs = Config::BUTTON_DEBOUNCE_MS,
  });

  g_led.setup(LedDriver::Config{
      .pin = Config::LED_PIN,
      .activeHigh = true,
  });

  FSM_State = LED_OFF_STATE;
  g_lastState = FSM_State;
  g_led.set(FSM[FSM_State].Out != 0);
  printf("LED: %s\n", FSM[FSM_State].Out ? "ON" : "OFF");
}

void loop()
{
  int output = FSM[FSM_State].Out;

  g_led.set(output != 0);

  delay(FSM[FSM_State].Time * 10);

  int input = g_button.pollPressEvent() ? 1 : 0;

  FSM_State = FSM[FSM_State].Next[input];

  if (FSM_State != g_lastState)
  {
    g_lastState = FSM_State;
    printf("LED: %s\n", FSM_State == LED_ON_STATE ? "ON" : "OFF");
  }
}