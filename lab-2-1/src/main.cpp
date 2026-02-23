#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#include "../lib/IO/IO.h"
#include "../lib/LedDriver/LedController.h"
#include "../lib/Scheduler/Scheduler.h"
#include "../lib/Tasks/Tasks.h"

void setup() {
  // redirect stdio to serial
  IO::setup();

  // initialize tasks
  tasksInit();

  // start scheduler (1ms system tick)
  Scheduler::getInstance().begin();

  printf("Ready\n");
}

void loop() {
  // run non-preemptive scheduler
  Scheduler::getInstance().run();
}