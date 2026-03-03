#include "task_stats.h"
#include "config.h"
#include "task_measure.h"
#include "LedController.h"

// Static statistics (protected by mutex)
Statistics_t s_xStats = {0, 0, 0, 0, 0};

// Static handles
static SemaphoreHandle_t s_xPressSemaphore = nullptr;
static SemaphoreHandle_t s_xStatsMutex = nullptr;

// LED instances - use pointers to avoid static initialization issues
static LedController *s_poGreenLed = nullptr;
static LedController *s_poRedLed = nullptr;
static LedController *s_poYellowLed = nullptr;

// Getters
uint32_t ulGetTotalPressCount()
{
  return s_xStats.ulTotalPressCount;
}

uint32_t ulGetShortPressCount()
{
  return s_xStats.ulShortPressCount;
}

uint32_t ulGetLongPressCount()
{
  return s_xStats.ulLongPressCount;
}

uint32_t ulGetSumShortDuration()
{
  return s_xStats.ulSumShortDuration;
}

uint32_t ulGetSumLongDuration()
{
  return s_xStats.ulSumLongDuration;
}

// Reset function
void vResetStatistics()
{
  if (xSemaphoreTake(s_xStatsMutex, portMAX_DELAY) == pdTRUE)
  {
    s_xStats.ulTotalPressCount = 0;
    s_xStats.ulShortPressCount = 0;
    s_xStats.ulLongPressCount = 0;
    s_xStats.ulSumShortDuration = 0;
    s_xStats.ulSumLongDuration = 0;
    xSemaphoreGive(s_xStatsMutex);
  }
}

// Blink yellow LED N times
static void prvBlinkYellowLed(uint8_t ucCount)
{
  for (uint8_t i = 0; i < ucCount; i++)
  {
    s_poYellowLed->turnOn();
    vTaskDelay(pdMS_TO_TICKS(YELLOW_BLINK_DELAY_MS));
    s_poYellowLed->turnOff();
    vTaskDelay(pdMS_TO_TICKS(YELLOW_BLINK_DELAY_MS));
  }
}

// Indicate press type with Green or Red LED
static void prvIndicatePressType(bool xIsLong)
{
  if (xIsLong)
  {
    // Long press - Red LED
    s_poRedLed->turnOn();
    vTaskDelay(pdMS_TO_TICKS(INDICATION_DURATION_MS));
    s_poRedLed->turnOff();
  }
  else
  {
    // Short press - Green LED
    s_poGreenLed->turnOn();
    vTaskDelay(pdMS_TO_TICKS(INDICATION_DURATION_MS));
    s_poGreenLed->turnOff();
  }
}

// Task function
static void prvTaskStats(void *pvParameters)
{
  // Create and setup LEDs
  s_poGreenLed = new LedController(GREEN_LED_PIN);
  s_poRedLed = new LedController(RED_LED_PIN);
  s_poYellowLed = new LedController(YELLOW_LED_PIN);

  s_poGreenLed->setup();
  s_poRedLed->setup();
  s_poYellowLed->setup();

  for (;;)
  {
    // Wait for press event from Task 1
    if (xSemaphoreTake(s_xPressSemaphore, portMAX_DELAY) == pdTRUE)
    {
      // Get press data
      uint32_t ulDuration = ulGetLastPressDuration();
      bool xIsLong = xGetLastPressIsLong();

      // Update statistics (protected by mutex)
      if (xSemaphoreTake(s_xStatsMutex, portMAX_DELAY) == pdTRUE)
      {
        s_xStats.ulTotalPressCount++;

        if (xIsLong)
        {
          s_xStats.ulLongPressCount++;
          s_xStats.ulSumLongDuration += ulDuration;
        }
        else
        {
          s_xStats.ulShortPressCount++;
          s_xStats.ulSumShortDuration += ulDuration;
        }

        xSemaphoreGive(s_xStatsMutex);
      }

      // Indicate press type with Green or Red LED
      prvIndicatePressType(xIsLong);

      // Blink yellow LED based on press type
      uint8_t ucBlinkCount = xIsLong ? 10 : 5;
      prvBlinkYellowLed(ucBlinkCount);
    }
  }
}

// Create function
void vTaskStatsCreate(SemaphoreHandle_t xPressSemaphore, SemaphoreHandle_t xStatsMutex)
{
  s_xPressSemaphore = xPressSemaphore;
  s_xStatsMutex = xStatsMutex;

  xTaskCreate(
      prvTaskStats,
      "TaskStats",
      TASK_STATS_STACK_SIZE,
      nullptr,
      TASK_STATS_PRIORITY,
      nullptr);
}
