#include "common.h"

#include <Arduino.h>
#include <stdio.h>

// Task function
void vTaskReport(void *pvParameters) {
    (void)pvParameters;

    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(REPORT_PERIOD_MS);

    while (1) {
        // Wait for next reporting period (accurate periodic execution)
        vTaskDelayUntil(&xLastWakeTime, xPeriod);

        // Get statistics with mutex protection
        Statistics local_stats;

        if (xSemaphoreTake(g_shared.stats_mutex, portMAX_DELAY) == pdTRUE) {
            // Copy statistics to local variable
            local_stats.total_presses = g_shared.stats.total_presses;
            local_stats.short_presses = g_shared.stats.short_presses;
            local_stats.long_presses = g_shared.stats.long_presses;
            local_stats.sum_short_duration = g_shared.stats.sum_short_duration;
            local_stats.sum_long_duration = g_shared.stats.sum_long_duration;

            // Reset statistics
            g_shared.stats.total_presses = 0;
            g_shared.stats.short_presses = 0;
            g_shared.stats.long_presses = 0;
            g_shared.stats.sum_short_duration = 0;
            g_shared.stats.sum_long_duration = 0;

            xSemaphoreGive(g_shared.stats_mutex);
        }

        // Calculate average duration
        float average_duration = 0.0f;
        if (local_stats.total_presses > 0) {
            uint32_t total_duration = local_stats.sum_short_duration + local_stats.sum_long_duration;
            average_duration = (float)total_duration / (float)local_stats.total_presses;
        }

        // Print statistics via STDIO
        printf("=== Statistics Report ===\n");
        printf("Total presses: %lu\n", (unsigned long)local_stats.total_presses);
        printf("Short presses: %lu\n", (unsigned long)local_stats.short_presses);
        printf("Long presses: %lu\n", (unsigned long)local_stats.long_presses);
        printf("Average duration: %.2f ms\n", average_duration);
        printf("=========================\n");
    }
}
