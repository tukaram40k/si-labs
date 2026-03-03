#include "task_report.h"
#include "config.h"
#include "task_stats.h"

// Static handle
static SemaphoreHandle_t s_xStatsMutex = nullptr;

// Task function
static void prvTaskReport(void *pvParameters)
{
  TickType_t xLastWakeTime;

  // Initialize serial for stdio (UART)
  Serial.begin(115200);
  delay(2000); // Give serial time to stabilize

  // Initialize last wake time to current time
  xLastWakeTime = xTaskGetTickCount();

  for (;;)
  {
    // Wait for next period (exactly 10 seconds)
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(TASK_REPORT_PERIOD_MS));

    // Acquire mutex to read AND reset statistics
    if (xSemaphoreTake(s_xStatsMutex, portMAX_DELAY) == pdTRUE)
    {
      uint32_t ulTotal = s_xStats.ulTotalPressCount;
      uint32_t ulShort = s_xStats.ulShortPressCount;
      uint32_t ulLong = s_xStats.ulLongPressCount;
      uint32_t ulSumShort = s_xStats.ulSumShortDuration;
      uint32_t ulSumLong = s_xStats.ulSumLongDuration;

      // Calculate average
      float fAverage = 0.0;
      if (ulTotal > 0)
      {
        fAverage = (float)(ulSumShort + ulSumLong) / (float)ulTotal;
      }

      // Print statistics using printf (redirected to UART)
      printf("========== STATISTICS REPORT ==========\n");
      printf("Total presses: %lu\n", ulTotal);
      printf("Short presses: %lu\n", ulShort);
      printf("Long presses: %lu\n", ulLong);
      printf("Average duration: %.2f ms\n", fAverage);
      printf("=======================================\n\n");

      // Reset statistics while still holding the mutex
      s_xStats.ulTotalPressCount = 0;
      s_xStats.ulShortPressCount = 0;
      s_xStats.ulLongPressCount = 0;
      s_xStats.ulSumShortDuration = 0;
      s_xStats.ulSumLongDuration = 0;

      xSemaphoreGive(s_xStatsMutex);
    }
  }
}

// Create function
void vTaskReportCreate(SemaphoreHandle_t xStatsMutex)
{
  s_xStatsMutex = xStatsMutex;

  xTaskCreate(
      prvTaskReport,
      "TaskReport",
      TASK_REPORT_STACK_SIZE,
      nullptr,
      TASK_REPORT_PRIORITY,
      nullptr);
}
