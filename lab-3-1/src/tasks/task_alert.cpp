#include "task_alert.h"
#include "../config.h"
#include "../shared_data.h"
#include "../services/alert_service.h"
#include "LedController.h"
#include <Arduino.h>

static alert_evaluator_t ds18b20_alert_eval;

static LedController *ledDS18B20 = nullptr;

void task_alert(void *pvParameters)
{
  xSemaphoreTake(g_start_gate, portMAX_DELAY);
  alert_evaluator_init(&ds18b20_alert_eval);

  ledDS18B20 = new LedController(PIN_LED_RED);
  ledDS18B20->setup();

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

    alert_state_t ds18b20_state = alert_evaluator_update(
        &ds18b20_alert_eval, local_processed.saturated_ds18b20_temp, now_ms);

    if (xSemaphoreTake(g_mutex_alert, pdMS_TO_TICKS(10)) == pdTRUE)
    {
      g_alert_data.ds18b20_alert = ds18b20_state;
      xSemaphoreGive(g_mutex_alert);
    }

    if (ds18b20_state == ALERT_ON) {
      ledDS18B20->turnOn();
    } else {
      ledDS18B20->turnOff();
    }

    vTaskDelayUntil(&xLastWakeTime, xPeriod);
  }
}