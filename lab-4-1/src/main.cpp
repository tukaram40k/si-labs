#include <Arduino.h>
#include <stdio.h>
#include "../lib/IO/IO.h"
#include "../lib/Actuator/Actuator.h"
#include "../lib/LedDriver/LedController.h"
#include "../lib/RemoteDriver/RemoteDriver.h"
#include "../lib/Scheduler/Scheduler.h"
#include "Tasks/task_actuator_control.h"
#include "Tasks/task_signal_conditioning.h"
#include "Tasks/task_reporting.h"

static constexpr uint8_t IR_RECEIVE_PIN = 2;
static constexpr uint8_t RELAY_PIN = 7;

static Actuator g_actuator({ .relayPin = RELAY_PIN, .activeHigh = true }, nullptr);

static RemoteDriver g_remote({
  .irReceivePin = IR_RECEIVE_PIN,
  .codeOn = 0x180C,
  .codeOff = 0x100C,
});

static void task_control_wrapper() { task_actuator_control_tick(); }
static void task_conditioning_wrapper() { task_signal_conditioning_tick(); }
static void task_reporting_wrapper() { task_reporting_tick(); }

static TaskItem g_tasks[] = {
  { .fn = task_control_wrapper, .periodMs = 50, .nextRunAt = 0 },
  { .fn = task_conditioning_wrapper, .periodMs = 50, .nextRunAt = 0 },
  { .fn = task_reporting_wrapper, .periodMs = 500, .nextRunAt = 0 },
};

static Scheduler g_scheduler(g_tasks, sizeof(g_tasks) / sizeof(g_tasks[0]));

void setup() {
  IO::setup();

  g_actuator.setup();
  g_remote.setup();

  task_actuator_control_setup();
  task_actuator_control_bind_remote(&g_remote);

  task_signal_conditioning_setup({ .debounceMs = 100, .requiredStableSamples = 2 }, &g_actuator);
  task_reporting_setup(&g_actuator);

  printf("\n====== Ready ======\n\n");

  g_scheduler.start(millis());
}

void loop() {
  g_scheduler.tick(millis());
}