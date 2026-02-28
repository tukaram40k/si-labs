#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <stdio.h>

#include "common.h"
#include "../lib/IO/IO.h"

// External task function declarations
extern void vTaskMeasure(void *pvParameters);
extern void vTaskStats(void *pvParameters);
extern void vTaskReport(void *pvParameters);

// Task stack sizes (in words, not bytes - Arduino_FreeRTOS uses words)
#define TASK_MEASURE_STACK_SIZE 256
#define TASK_STATS_STACK_SIZE 256
#define TASK_REPORT_STACK_SIZE 256

// Task priorities (higher number = higher priority)
#define TASK_MEASURE_PRIORITY 2
#define TASK_STATS_PRIORITY 1
#define TASK_REPORT_PRIORITY 1

// Global shared state
SharedState g_shared;

// Initialize shared state (semaphores, mutex, variables)
void sharedStateInit(void) {
    // Create binary semaphore for press event signaling (Task1 -> Task2)
    g_shared.press_semaphore = xSemaphoreCreateBinary();
    
    // Create mutex for statistics protection
    g_shared.stats_mutex = xSemaphoreCreateMutex();

    // Initialize statistics
    g_shared.stats.total_presses = 0;
    g_shared.stats.short_presses = 0;
    g_shared.stats.long_presses = 0;
    g_shared.stats.sum_short_duration = 0;
    g_shared.stats.sum_long_duration = 0;

    // Initialize shared data
    g_shared.last_duration_ms = 0;
    g_shared.last_press_type = PRESS_NONE;
    g_shared.green_led_on = false;
    g_shared.red_led_on = false;
}

void setup() {
    // Initialize IO (redirect stdio to serial)
    IO::setup();

    // Wait for serial to be ready
    while (!Serial) {
        ;
    }

    printf("Initializing FreeRTOS...\n");

    // Initialize shared state (semaphores, mutex)
    sharedStateInit();

    // Verify semaphores were created
    if (g_shared.press_semaphore == NULL || g_shared.stats_mutex == NULL) {
        printf("ERROR: Failed to create semaphores!\n");
        while (1) {
            ;
        }
    }

    printf("Creating tasks...\n");

    // Create Task 1 - Button Detection & Duration Measurement
    BaseType_t result1 = xTaskCreate(
        vTaskMeasure,           // Task function
        "TaskMeasure",          // Task name
        TASK_MEASURE_STACK_SIZE,// Stack size (words)
        NULL,                   // Parameters
        TASK_MEASURE_PRIORITY,  // Priority
        NULL                    // Task handle
    );

    if (result1 != pdPASS) {
        printf("ERROR: Failed to create TaskMeasure (error %d)\n", result1);
    }

    // Create Task 2 - Statistics & Yellow LED Blink
    BaseType_t result2 = xTaskCreate(
        vTaskStats,             // Task function
        "TaskStats",            // Task name
        TASK_STATS_STACK_SIZE,  // Stack size (words)
        NULL,                   // Parameters
        TASK_STATS_PRIORITY,    // Priority
        NULL                    // Task handle
    );

    if (result2 != pdPASS) {
        printf("ERROR: Failed to create TaskStats (error %d)\n", result2);
    }

    // Create Task 3 - Periodic Reporting
    BaseType_t result3 = xTaskCreate(
        vTaskReport,            // Task function
        "TaskReport",           // Task name
        TASK_REPORT_STACK_SIZE, // Stack size (words)
        NULL,                   // Parameters
        TASK_REPORT_PRIORITY,   // Priority
        NULL                    // Task handle
    );

    if (result3 != pdPASS) {
        printf("ERROR: Failed to create TaskReport (error %d)\n", result3);
    }

    printf("Starting scheduler...\n");

    // Start FreeRTOS scheduler (this will never return)
    vTaskStartScheduler();

    // Should never reach here
    printf("ERROR: Scheduler returned unexpectedly!\n");
}

void loop() {
    // Empty - FreeRTOS scheduler handles all tasks
    // This function is never reached when vTaskStartScheduler() succeeds
}
