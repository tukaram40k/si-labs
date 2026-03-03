#ifndef TASK_MEASURE_H
#define TASK_MEASURE_H

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// Function to create the measure task
void vTaskMeasureCreate(SemaphoreHandle_t xPressSemaphore, SemaphoreHandle_t xDurationMutex);

// Getters for shared data (protected by mutex)
uint32_t ulGetLastPressDuration();
bool xGetLastPressIsLong();

#endif // TASK_MEASURE_H
