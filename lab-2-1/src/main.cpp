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
  
  // Start the scheduler (enables Timer1 interrupt for 1ms system tick)
  Scheduler::getInstance().begin();
  
  printf("Ready\n");
}

void loop() {
  // Run the non-preemptive scheduler
  // Iterates through all registered tasks and executes those that are due
  Scheduler::getInstance().run();
}