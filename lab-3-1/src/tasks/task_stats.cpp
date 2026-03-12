#include "task_stats.h"
#include "config.h"
#include "task_measure.h"
#include "LedController.h"

// Static statistics (protected by mutex)
Statistics_t Stats = {0, 0, 0, 0, 0};

// Static handles
static SemaphoreHandle_t PressSemaphore = nullptr;
static SemaphoreHandle_t StatsMutex = nullptr;

// LED instances - use pointers to avoid static initialization issues
static LedController *GreenLed = nullptr;
static LedController *RedLed = nullptr;
static LedController *YellowLed = nullptr;

// Getters
uint32_t ulGetTotalPressCount()
{
  return Stats.ulTotalPressCount;
}

uint32_t ulGetShortPressCount()
{
  return Stats.ulShortPressCount;
}

uint32_t ulGetLongPressCount()
{
  return Stats.ulLongPressCount;
}

uint32_t ulGetSumShortDuration()
{
  return Stats.ulSumShortDuration;
}

uint32_t ulGetSumLongDuration()
{
  return Stats.ulSumLongDuration;
}

// Reset function
void vResetStatistics()
{
  if (xSemaphoreTake(StatsMutex, portMAX_DELAY) == pdTRUE)
  {
    Stats.ulTotalPressCount = 0;
    Stats.ulShortPressCount = 0;
    Stats.ulLongPressCount = 0;
    Stats.ulSumShortDuration = 0;
    Stats.ulSumLongDuration = 0;
    xSemaphoreGive(StatsMutex);
  }
}

// Blink yellow LED N times
static void prvBlinkYellowLed(uint8_t ucCount)
{
  for (uint8_t i = 0; i < ucCount; i++)
  {
    YellowLed->turnOn();
    vTaskDelay(pdMS_TO_TICKS(YELLOW_BLINK_DELAY_MS));
    YellowLed->turnOff();
    vTaskDelay(pdMS_TO_TICKS(YELLOW_BLINK_DELAY_MS));
  }
}

// Indicate press type with Green or Red LED
static void prvIndicatePressType(bool xIsLong)
{
  if (xIsLong)
  {
    // Long press - Red LED
    RedLed->turnOn();
    vTaskDelay(pdMS_TO_TICKS(INDICATION_DURATION_MS));
    RedLed->turnOff();
  }
  else
  {
    // Short press - Green LED
    GreenLed->turnOn();
    vTaskDelay(pdMS_TO_TICKS(INDICATION_DURATION_MS));
    GreenLed->turnOff();
  }
}

// Task function
static void prvTaskStats(void *pvParameters)
{
  // Create and setup LEDs
  GreenLed = new LedController(GREEN_LED_PIN);
  RedLed = new LedController(RED_LED_PIN);
  YellowLed = new LedController(YELLOW_LED_PIN);

  GreenLed->setup();
  RedLed->setup();
  YellowLed->setup();

  for (;;)
  {
    // Wait for press event from Task 1
    if (xSemaphoreTake(PressSemaphore, portMAX_DELAY) == pdTRUE)
    {
      // Get press data
      uint32_t ulDuration = ulGetLastPressDuration();
      bool xIsLong = xGetLastPressIsLong();

      // Update statistics (protected by mutex)
      if (xSemaphoreTake(StatsMutex, portMAX_DELAY) == pdTRUE)
      {
        Stats.ulTotalPressCount++;

        if (xIsLong)
        {
          Stats.ulLongPressCount++;
          Stats.ulSumLongDuration += ulDuration;
        }
        else
        {
          Stats.ulShortPressCount++;
          Stats.ulSumShortDuration += ulDuration;
        }

        xSemaphoreGive(StatsMutex);
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
  PressSemaphore = xPressSemaphore;
  StatsMutex = xStatsMutex;

  xTaskCreate(
      prvTaskStats,
      "TaskStats",
      TASK_STATS_STACK_SIZE,
      nullptr,
      TASK_STATS_PRIORITY,
      nullptr);
}
