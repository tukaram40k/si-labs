#include "common.h"
#include "../lib/LedDriver/LedController.h"

#include <Arduino.h>

// Yellow LED instance
static LedController g_led_yellow(PIN_LED_YELLOW);

// Task function
void vTaskStats(void *pvParameters) {
    (void)pvParameters;

    // Initialize hardware
    g_led_yellow.setup();

    while (1) {
        // Wait for press event from Task 1
        if (xSemaphoreTake(g_shared.press_semaphore, portMAX_DELAY) == pdTRUE) {
            
            // Get press type and duration with mutex protection
            PressType press_type = PRESS_NONE;
            uint32_t duration_ms = 0;

            if (xSemaphoreTake(g_shared.stats_mutex, portMAX_DELAY) == pdTRUE) {
                press_type = g_shared.last_press_type;
                duration_ms = g_shared.last_duration_ms;
                xSemaphoreGive(g_shared.stats_mutex);
            }

            // Update statistics
            if (xSemaphoreTake(g_shared.stats_mutex, portMAX_DELAY) == pdTRUE) {
                g_shared.stats.total_presses++;

                if (press_type == PRESS_SHORT) {
                    g_shared.stats.short_presses++;
                    g_shared.stats.sum_short_duration += duration_ms;
                } else if (press_type == PRESS_LONG) {
                    g_shared.stats.long_presses++;
                    g_shared.stats.sum_long_duration += duration_ms;
                }

                xSemaphoreGive(g_shared.stats_mutex);
            }

            // Determine blink count based on press type
            uint8_t blink_count = (press_type == PRESS_SHORT) 
                ? SHORT_BLINK_COUNT : LONG_BLINK_COUNT;

            // Perform yellow LED blinking
            for (uint8_t i = 0; i < blink_count; i++) {
                g_led_yellow.turnOn();
                vTaskDelay(pdMS_TO_TICKS(YELLOW_BLINK_PERIOD_MS));
                g_led_yellow.turnOff();
                vTaskDelay(pdMS_TO_TICKS(YELLOW_BLINK_PERIOD_MS));
            }
        }
    }
}
