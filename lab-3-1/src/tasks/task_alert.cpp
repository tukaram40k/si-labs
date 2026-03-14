#include "task_alert.h"
#include "../config.h"
#include "../shared_data.h"
#include "../services/alert_service.h"
#include "LedController.h"
#include <Arduino.h>

// Independent alert evaluators for each sensor
static alert_evaluator_t ntc_alert_eval;
static alert_evaluator_t ds18b20_alert_eval;

// LED controllers for each sensor alert
static LedController *ledNTC = nullptr;   // Red LED - NTC alert
static LedController *ledDS18B20 = nullptr; // Green LED - DS18B20 alert

void task_alert(void *pvParameters)
{
  // Wait for start gate — ensures setup() has finished
  xSemaphoreTake(g_start_gate, portMAX_DELAY);
  printf("[ALERT] Task started on core %d\n", xPortGetCoreID());

  alert_evaluator_init(&ntc_alert_eval);
  alert_evaluator_init(&ds18b20_alert_eval);

  // Initialize LED controllers
  ledNTC = new LedController(PIN_LED_RED);
  ledDS18B20 = new LedController(PIN_LED_GREEN);
  ledNTC->setup();
  ledDS18B20->setup();
  printf("[ALERT] LEDs configured: GREEN=GPIO%d (DS18B20), RED=GPIO%d (NTC)\n", PIN_LED_GREEN, PIN_LED_RED);

  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xPeriod = pdMS_TO_TICKS(TASK_ALERT_PERIOD_MS);

  for (;;)
  {
    uint32_t now_ms = (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);

    // --- Read processed data (mutex-protected) ---
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

    // --- Evaluate alerts for each sensor ---
    alert_state_t ntc_state = alert_evaluator_update(
        &ntc_alert_eval, local_processed.filtered_ntc_temp, now_ms);

    alert_state_t ds18b20_state = alert_evaluator_update(
        &ds18b20_alert_eval, local_processed.filtered_ds18b20_temp, now_ms);

    // --- Store alert states (mutex-protected) ---
    if (xSemaphoreTake(g_mutex_alert, pdMS_TO_TICKS(10)) == pdTRUE)
    {
      g_alert_data.ntc_alert = ntc_state;
      g_alert_data.ds18b20_alert = ds18b20_state;
      xSemaphoreGive(g_mutex_alert);
    }

    // --- Drive LEDs ---
    // Each sensor has its own LED for alert indication
    // Red LED: NTC (analog) sensor alert
    // Green LED: DS18B20 (digital) sensor alert
    // LEDs are OFF when no alert, ON when alert is active
    if (ntc_state == ALERT_ON) {
      ledNTC->turnOn();
    } else {
      ledNTC->turnOff();
    }
    if (ds18b20_state == ALERT_ON) {
      ledDS18B20->turnOn();
    } else {
      ledDS18B20->turnOff();
    }

    vTaskDelayUntil(&xLastWakeTime, xPeriod);
  }
}