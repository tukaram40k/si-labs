#ifndef TASK_REPORT_H
#define TASK_REPORT_H

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// This file is no longer used. See task_display.h

// Function to create the report task
void vTaskReportCreate(SemaphoreHandle_t xStatsMutex);

#endif // TASK_REPORT_H
