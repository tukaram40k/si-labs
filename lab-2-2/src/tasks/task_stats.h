#ifndef TASK_STATS_H
#define TASK_STATS_H

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// Statistics structure (for internal use)
typedef struct
{
  uint32_t ulTotalPressCount;
  uint32_t ulShortPressCount;
  uint32_t ulLongPressCount;
  uint32_t ulSumShortDuration;
  uint32_t ulSumLongDuration;
} Statistics_t;

// External statistics reference
extern Statistics_t s_xStats;

// Function to create the stats task
void vTaskStatsCreate(SemaphoreHandle_t xPressSemaphore, SemaphoreHandle_t xStatsMutex);

// Statistics getters (protected by mutex)
uint32_t ulGetTotalPressCount();
uint32_t ulGetShortPressCount();
uint32_t ulGetLongPressCount();
uint32_t ulGetSumShortDuration();
uint32_t ulGetSumLongDuration();

// Reset statistics (protected by mutex)
void vResetStatistics();

#endif // TASK_STATS_H
