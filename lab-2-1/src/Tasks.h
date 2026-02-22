#ifndef TASKS_H
#define TASKS_H

#include <Arduino.h>
#include <stdint.h>

// Pin definitions
#define BUTTON_PIN 2
#define LED1_PIN 12  // Status/toggle LED (red)
#define LED2_PIN 11  // Rapid blink LED (green)
#define LED3_PIN 10  // Heartbeat/auxiliary LED (yellow)

// Task periods in milliseconds
#define TASK1_PERIOD_MS 50       // Button detector (50ms for debouncing)
#define TASK2_PERIOD_MS 10       // Press counter & visualizer (fast response)
#define TASK3_PERIOD_MS 10000    // Status monitor (10 seconds)

// Task offsets to prevent collision
#define TASK1_OFFSET_MS 0
#define TASK2_OFFSET_MS 5
#define TASK3_OFFSET_MS 100

// Debounce configuration
#define DEBOUNCE_DELAY_MS 50     // Debounce time in milliseconds

// Rapid blink configuration
#define RAPID_BLINK_COUNT 20     // Number of state changes (5 full blinks)
#define RAPID_BLINK_PERIOD_MS 100 // 200ms per state change (visible blink)

// Shared state variables (volatile for ISR safety)
extern volatile uint32_t pushCount;      // Global push counter
extern volatile bool triggerRapidBlink;  // Flag to trigger rapid blink sequence

// Task function declarations
void task1_ButtonDetector(void);
void task2_PressCounterVisualizer(void);
void task3_StatusMonitor(void);

// Task initialization
void tasksInit(void);

#endif // TASKS_H
