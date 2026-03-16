#include "task_alert.h"
#include "../config.h"
#include "../shared_data.h"
#include "../services/alert_service.h"
#include "LedController.h"
#include <Arduino.h>

static alert_evaluator_t ntc_alert_eval;
static LedController *ledNTC = nullptr;

void task_alert(void *pvParameters)
{
  xSemaphoreTake(g_start_gate, portMAX_DELAY);
  alert_evaluator_init(&ntc_alert_eval);

  ledNTC = new LedController(PIN_LED_RED);
  ledNTC->setup();

  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xPeriod = pdMS_TO_TICKS(TASK_ALERT_PERIOD_MS);

  for (;;)
  {
    uint32_t now_ms = (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);

    processed_data_t local_processed;
    if (xSemaphoreTake(g_mutex_processed, pdMS_TO_TICKS(10)) == pdTRUE)
    {
      local_processed = g_processed_data;
      xSemaphoreGive(g_mutex_processed);
    }
    else
    {
      vTaskDelayUntil(&xLastWakeTime, xPeriod);
      continue;
    }

    alert_state_t ntc_state = alert_evaluator_update(
        &ntc_alert_eval, local_processed.filtered_ntc_temp, now_ms);

    if (xSemaphoreTake(g_mutex_alert, pdMS_TO_TICKS(10)) == pdTRUE)
    {
      g_alert_data.ntc_alert = ntc_state;
      xSemaphoreGive(g_mutex_alert);
    }

    if (ntc_state == ALERT_ON) {
      ledNTC->turnOn();
    } else {
      ledNTC->turnOff();
    }

    vTaskDelayUntil(&xLastWakeTime, xPeriod);
  }
}