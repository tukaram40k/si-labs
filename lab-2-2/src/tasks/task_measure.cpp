#include "task_measure.h"
#include "config.h"
#include "ButtonController.h"

// Static variables for shared data (protected by mutex)
static uint32_t s_ulLastPressDuration = 0;
static bool s_xLastPressIsLong = false;

// Static handles
static SemaphoreHandle_t s_xPressSemaphore = nullptr;
static SemaphoreHandle_t s_xDurationMutex = nullptr;

// Button instance - use pointer to avoid static initialization issues
static ButtonController *s_poButton = nullptr;

// Getters
uint32_t ulGetLastPressDuration()
{
  return s_ulLastPressDuration;
}

bool xGetLastPressIsLong()
{
  return s_xLastPressIsLong;
}

// Task function
static void prvTaskMeasure(void *pvParameters)
{
  bool xWasPressed = false;
  uint32_t ulPressStartTime = 0;

  // Create and setup button
  s_poButton = new ButtonController(BUTTON_PIN, true);
  s_poButton->setup();

  for (;;)
  {
    bool xIsPressed = s_poButton->isPressed();
    uint32_t ulCurrentTime = millis();

    // Detect transition: RELEASED -> PRESSED
    if (xIsPressed && !xWasPressed)
    {
      ulPressStartTime = ulCurrentTime;
    }

    // Detect transition: PRESSED -> RELEASED
    if (!xIsPressed && xWasPressed)
    {
      uint32_t ulDuration = ulCurrentTime - ulPressStartTime;

      // Acquire mutex to protect shared data
      if (xSemaphoreTake(s_xDurationMutex, portMAX_DELAY) == pdTRUE)
      {
        s_ulLastPressDuration = ulDuration;
        s_xLastPressIsLong = (ulDuration >= SHORT_PRESS_THRESHOLD_MS);
        xSemaphoreGive(s_xDurationMutex);
      }

      // Signal Task 2 that a press event occurred
      xSemaphoreGive(s_xPressSemaphore);
    }

    xWasPressed = xIsPressed;

    // Delay for polling period
    vTaskDelay(pdMS_TO_TICKS(TASK_MEASURE_PERIOD_MS));
  }
}

// Create function
void vTaskMeasureCreate(SemaphoreHandle_t xPressSemaphore, SemaphoreHandle_t xDurationMutex)
{
  s_xPressSemaphore = xPressSemaphore;
  s_xDurationMutex = xDurationMutex;

  xTaskCreate(
      prvTaskMeasure,
      "TaskMeasure",
      TASK_MEASURE_STACK_SIZE,
      nullptr,
      TASK_MEASURE_PRIORITY,
      nullptr);
}
