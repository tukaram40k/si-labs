#include "task_report.h"
#include "config.h"
#include "task_stats.h"
#include "LCDController.h"

// Static handle
static SemaphoreHandle_t StatsMutex = nullptr;

// LCD instance (I2C address 0x27, 16 columns, 2 rows)
static LCDController* lcd = nullptr;

// Task function
static void prvTaskReport(void *pvParameters)
{
  TickType_t xLastWakeTime;

  // Initialize LCD (I2C address 0x27, 16x2 display)
  lcd = new LCDController(0x27, 16, 2);
  lcd->setup();

  // Initialize last wake time to current time
  xLastWakeTime = xTaskGetTickCount();

  for (;;)
  {
    // Wait for next period (exactly 10 seconds)
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(TASK_REPORT_PERIOD_MS));

    // Acquire mutex to read AND reset statistics
    if (xSemaphoreTake(StatsMutex, portMAX_DELAY) == pdTRUE)
    {
      uint32_t ulTotal = Stats.ulTotalPressCount;
      uint32_t ulShort = Stats.ulShortPressCount;
      uint32_t ulLong = Stats.ulLongPressCount;
      uint32_t ulSumShort = Stats.ulSumShortDuration;
      uint32_t ulSumLong = Stats.ulSumLongDuration;

      // Calculate average
      float fAverage = 0.0;
      if (ulTotal > 0)
      {
        fAverage = (float)(ulSumShort + ulSumLong) / (float)ulTotal;
      }

      // Print statistics to stdio (UART)
      printf("=======================================\n");
      printf("Total presses: %lu\n", ulTotal);
      printf("Short presses: %lu\n", ulShort);
      printf("Long presses: %lu\n", ulLong);
      printf("Average duration: %.2f ms\n", fAverage);
      printf("=======================================\n\n");

      // Display statistics on LCD
      lcd->clear();
      
      // Line 1: S:Y L:Z
      lcd->printAt(0, 0, "T: ");
      lcd->print(String(ulTotal).c_str());
      lcd->print(" S: ");
      lcd->print(String(ulShort).c_str());
      lcd->print(" L: ");
      lcd->print(String(ulLong).c_str());
      
      // Line 2: Avg: X.XX ms
      lcd->printAt(0, 1, "Avg: ");
      lcd->print(String(fAverage, 2).c_str());
      lcd->print(" ms");

      // Reset statistics while still holding the mutex
      Stats.ulTotalPressCount = 0;
      Stats.ulShortPressCount = 0;
      Stats.ulLongPressCount = 0;
      Stats.ulSumShortDuration = 0;
      Stats.ulSumLongDuration = 0;

      xSemaphoreGive(StatsMutex);
    }
  }
}

// Create function
void vTaskReportCreate(SemaphoreHandle_t xStatsMutex)
{
  StatsMutex = xStatsMutex;

  xTaskCreate(
      prvTaskReport,
      "TaskReport",
      TASK_REPORT_STACK_SIZE,
      nullptr,
      TASK_REPORT_PRIORITY,
      nullptr);
}
