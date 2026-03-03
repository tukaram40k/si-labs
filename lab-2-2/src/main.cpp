#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "config.h"
#include "tasks/task_measure.h"
#include "tasks/task_stats.h"
#include "tasks/task_report.h"

// semaphore and mutex handles
static SemaphoreHandle_t s_xPressSemaphore = nullptr;
static SemaphoreHandle_t s_xStatsMutex = nullptr;

void setup() {
  // semaphore for button press event
  s_xPressSemaphore = xSemaphoreCreateBinary();

  // mutex for stats
  s_xStatsMutex = xSemaphoreCreateMutex();

  // tasks
  vTaskMeasureCreate(s_xPressSemaphore, s_xStatsMutex);
  vTaskStatsCreate(s_xPressSemaphore, s_xStatsMutex);
  vTaskReportCreate(s_xStatsMutex);

  // start scheduler
  vTaskStartScheduler();
}

void loop() {

}
