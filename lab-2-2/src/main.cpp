#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "config.h"
#include "tasks/task_measure.h"
#include "tasks/task_stats.h"
#include "tasks/task_report.h"

// semaphore and mutex handles
static SemaphoreHandle_t PressSemaphore = nullptr;
static SemaphoreHandle_t StatsMutex = nullptr;

void setup() {
  // semaphore for button press event
  PressSemaphore = xSemaphoreCreateBinary();

  // mutex for stats
  StatsMutex = xSemaphoreCreateMutex();

  // tasks
  vTaskMeasureCreate(PressSemaphore, StatsMutex);
  vTaskStatsCreate(PressSemaphore, StatsMutex);
  vTaskReportCreate(StatsMutex);

  // start scheduler
  vTaskStartScheduler();
}

void loop() {

}
