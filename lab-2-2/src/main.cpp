#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "config.h"
#include "task_measure.h"
#include "task_stats.h"
#include "task_report.h"

// Semaphore and mutex handles
static SemaphoreHandle_t s_xPressSemaphore = nullptr;
static SemaphoreHandle_t s_xStatsMutex = nullptr;

void setup() {
    // Create binary semaphore for press event signaling (Task 1 -> Task 2)
    s_xPressSemaphore = xSemaphoreCreateBinary();
    
    // Create mutex for protecting shared statistics data
    s_xStatsMutex = xSemaphoreCreateMutex();
    
    // Create Task 1: Button Detection & Duration Measurement
    vTaskMeasureCreate(s_xPressSemaphore, s_xStatsMutex);
    
    // Create Task 2: Statistics & Yellow LED Blink
    vTaskStatsCreate(s_xPressSemaphore, s_xStatsMutex);
    
    // Create Task 3: Periodic Reporting
    vTaskReportCreate(s_xStatsMutex);
    
    // Start the scheduler - this will never return
    vTaskStartScheduler();
}

void loop() {
    // This will never be called
}
