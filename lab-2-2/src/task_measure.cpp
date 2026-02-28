#include "common.h"
#include "../lib/ButtonDriver/ButtonController.h"
#include "../lib/LedDriver/LedController.h"

#include <Arduino.h>

// Button and LED instances
static ButtonController g_button(PIN_BUTTON);
static LedController g_led_green(PIN_LED_GREEN);
static LedController g_led_red(PIN_LED_RED);

// Debounce state
static uint8_t s_debounce_counter = 0;
static bool s_last_button_state = false;

// Press measurement state
static bool s_press_started = false;
static uint32_t s_press_start_time = 0;

// LED timing state
static uint32_t s_led_on_time = 0;
static bool s_led_timer_active = false;

// Task function
void vTaskMeasure(void *pvParameters) {
    (void)pvParameters;

    // Initialize hardware
    g_button.setup();
    g_led_green.setup();
    g_led_red.setup();

    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(BUTTON_POLL_PERIOD_MS);

    while (1) {
        // Wait for next polling period
        vTaskDelayUntil(&xLastWakeTime, xPeriod);

        // Read button state (active low)
        bool current_state = g_button.isPressed();

        // Debounce logic
        if (current_state != s_last_button_state) {
            // State changed, reset debounce counter
            s_debounce_counter = 0;
            s_last_button_state = current_state;
        } else {
            // State stable, increment counter
            if (s_debounce_counter < DEBOUNCE_STABLE_COUNT) {
                s_debounce_counter++;
            }

            // Check for stable press transition (RELEASED -> PRESSED)
            if (s_debounce_counter == DEBOUNCE_STABLE_COUNT && 
                current_state && 
                !s_press_started) {
                
                // Press detected - record start time
                s_press_started = true;
                s_press_start_time = xTaskGetTickCount();
            }
            // Check for stable release transition (PRESSED -> RELEASED)
            else if (s_debounce_counter == DEBOUNCE_STABLE_COUNT && 
                     !current_state && 
                     s_press_started) {
                
                // Release detected - calculate duration
                uint32_t current_time = xTaskGetTickCount();
                uint32_t duration_ticks = current_time - s_press_start_time;
                uint32_t duration_ms = duration_ticks * portTICK_PERIOD_MS;

                // Classify press type
                PressType press_type = (duration_ms < PRESS_DURATION_THRESHOLD_MS) 
                    ? PRESS_SHORT : PRESS_LONG;

                // Update shared state with mutex protection
                if (xSemaphoreTake(g_shared.stats_mutex, portMAX_DELAY) == pdTRUE) {
                    g_shared.last_duration_ms = duration_ms;
                    g_shared.last_press_type = press_type;
                    xSemaphoreGive(g_shared.stats_mutex);
                }

                // Signal Task 2 (Stats) about new press event
                xSemaphoreGive(g_shared.press_semaphore);

                // Turn on appropriate LED
                if (press_type == PRESS_SHORT) {
                    g_led_green.turnOn();
                } else {
                    g_led_red.turnOn();
                }

                // Record LED on time
                s_led_on_time = xTaskGetTickCount();
                s_led_timer_active = true;

                // Reset press state
                s_press_started = false;
            }
        }

        // Turn off Green/Red LED after indication period
        if (g_led_green.isOn() || g_led_red.isOn()) {
            uint32_t elapsed = xTaskGetTickCount() - s_led_on_time;
            if (elapsed >= pdMS_TO_TICKS(GREEN_RED_INDICATION_MS)) {
                g_led_green.turnOff();
                g_led_red.turnOff();
                s_led_timer_active = false;
            }
        } else {
            s_led_timer_active = false;
        }
    }
}
