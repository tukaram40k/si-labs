#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#include "../lib/IO/IO.h"
#include "../lib/LedDriver/LedController.h"
#include "../lib/Scheduler/Scheduler.h"
#include "../lib/Tasks/Tasks.h"

// create led controllers (kept for compatibility, tasks use direct pin access)
LedController led1(12);
LedController led2(11);
LedController led3(10);

void setup() {
  // Redirect stdio to serial first
  IO::setup();
  
  // Initialize tasks (configures pins and registers with scheduler)
  tasksInit();
  
  // Start the scheduler (enables Timer1 interrupt for 1ms system tick)
  Scheduler::getInstance().begin();
  
  printf("Ready\n");
  printf("Sequential Task Scheduler Started\n");
  printf("Task 1: Button Detector (10ms period)\n");
  printf("Task 2: Press Counter (50ms period)\n");
  printf("Task 3: Status Monitor (10s period)\n");
}

void loop() {
  // Run the non-preemptive scheduler
  // Iterates through all registered tasks and executes those that are due
  Scheduler::getInstance().run();
}