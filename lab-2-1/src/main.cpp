#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#include "../lib/IO/IO.h"
#include "../lib/LedDriver/LedController.h"
#include "Scheduler.h"
#include "Tasks.h"

// create led controllers (kept for compatibility, tasks use direct pin access)
LedController led1(12);
LedController led2(11);
LedController led3(10);

void setup() {
  // Initialize LED controllers
  led1.setup();
  led2.setup();
  led3.setup();

  // Redirect stdio to serial
  IO::setup();
  printf("=== Sequential Task Scheduler Started ===\n");

  // Initialize the scheduler
  Scheduler::init();

  // Initialize tasks (configures pins and state variables)
  tasksInit();

  // Register tasks with the scheduler
  // Task 1: Button Detector - runs every 50ms, no offset
  Scheduler::addTask(task1_ButtonDetector, TASK1_PERIOD_MS, TASK1_OFFSET_MS);

  // Task 2: Press Counter & Visualizer - runs every 10ms, 5ms offset
  Scheduler::addTask(task2_PressCounterVisualizer, TASK2_PERIOD_MS, TASK2_OFFSET_MS);

  // Task 3: Status Monitor - runs every 10 seconds, 100ms offset
  Scheduler::addTask(task3_StatusMonitor, TASK3_PERIOD_MS, TASK3_OFFSET_MS);

  printf("Tasks registered: %d\n", Scheduler::getTaskCount());
  printf("Task 1 (Button Detector): period=%dms, offset=%dms\n", TASK1_PERIOD_MS, TASK1_OFFSET_MS);
  printf("Task 2 (Press Counter): period=%dms, offset=%dms\n", TASK2_PERIOD_MS, TASK2_OFFSET_MS);
  printf("Task 3 (Status Monitor): period=%dms, offset=%dms\n", TASK3_PERIOD_MS, TASK3_OFFSET_MS);

  // Start the scheduler (enables 1ms timer interrupt)
  Scheduler::start();

  printf("Scheduler started. System tick running.\n");
  printf("=========================================\n\n");
}

void loop() {
  // Run the scheduler - executes all due tasks
  // This is non-blocking as each task is a state machine
  Scheduler::run();

  // Main loop is free for other background operations
  // The scheduler handles all time-critical tasks via the 1ms timer interrupt
}