#include "task_alert.h"
#include "../config.h"
#include "../shared_data.h"
#include "../alert_service.h"
#include <Arduino.h>

// Independent alert evaluators for each sensor
static alert_evaluator_t ntc_alert_eval;
static alert_evaluator_t ds18b20_alert_eval;

void task_alert(void *pvParameters) {
    printf("[ALERT] Task started on core %d\n", xPortGetCoreID());

    alert_evaluator_init(&ntc_alert_eval);
    alert_evaluator_init(&ds18b20_alert_eval);

    // Configure LED pins for visual alert indication
    pinMode(PIN_LED_GREEN, OUTPUT);
    pinMode(PIN_LED_RED, OUTPUT);
    printf("[ALERT] LEDs configured: GREEN=GPIO%d, RED=GPIO%d\n", PIN_LED_GREEN, PIN_LED_RED);

    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(TASK_ALERT_PERIOD_MS);

    for (;;) {
        uint32_t now_ms = (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);

        // --- Read processed data (mutex-protected) ---
        processed_data_t local_processed;
        if (xSemaphoreTake(g_mutex_processed, pdMS_TO_TICKS(10)) == pdTRUE) {
            local_processed = g_processed_data;
            xSemaphoreGive(g_mutex_processed);
        } else {
            vTaskDelayUntil(&xLastWakeTime, xPeriod);
            continue;
        }

        // --- Evaluate alerts for each sensor ---
        alert_state_t ntc_state = alert_evaluator_update(
            &ntc_alert_eval, local_processed.filtered_ntc_temp, now_ms);

        alert_state_t ds18b20_state = alert_evaluator_update(
            &ds18b20_alert_eval, local_processed.filtered_ds18b20_temp, now_ms);

        // --- Store alert states (mutex-protected) ---
        if (xSemaphoreTake(g_mutex_alert, pdMS_TO_TICKS(10)) == pdTRUE) {
            g_alert_data.ntc_alert     = ntc_state;
            g_alert_data.ds18b20_alert = ds18b20_state;
            xSemaphoreGive(g_mutex_alert);
        }

        // --- Drive LEDs ---
        // Green LED: system OK (both alerts off)
        // Red LED: any alert active
        bool anyAlert = (ntc_state == ALERT_ON || ds18b20_state == ALERT_ON);
        digitalWrite(PIN_LED_GREEN, anyAlert ? LOW : HIGH);
        digitalWrite(PIN_LED_RED,   anyAlert ? HIGH : LOW);

        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}