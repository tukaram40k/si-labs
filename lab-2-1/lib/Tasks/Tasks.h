#ifndef TASKS_H
#define TASKS_H

#include <stdint.h>
#include <stdbool.h>

// ============================================================================
// Hardware Pin Definitions (from diagram.json)
// ============================================================================
#define PIN_LED1 12  // Red LED - Status/Toggle
#define PIN_LED2 11  // Green LED - Rapid blink sequence
#define PIN_LED3 10  // Yellow LED - Power/Heartbeat
#define PIN_BUTTON 2 // Push button input

// ============================================================================
// Task Timing Configuration
// ============================================================================
// Task 1: Button Detector - runs every 10ms for responsive detection (<100ms latency)
#define TASK1_PERIOD_MS 10
#define TASK1_OFFSET_MS 0

// Task 2: Press Counter & Visualizer - runs every 50ms
#define TASK2_PERIOD_MS 50
#define TASK2_OFFSET_MS 5

// Task 3: Status Monitor - runs every 10 seconds
#define TASK3_PERIOD_MS 10000
#define TASK3_OFFSET_MS 1000

// ============================================================================
// Debounce Configuration
// ============================================================================
#define DEBOUNCE_DELAY_MS 50 // Debounce delay for button
#define DEBOUNCE_STABLE_COUNT (DEBOUNCE_DELAY_MS / TASK1_PERIOD_MS)

// ============================================================================
// Blink Sequence Configuration
// ============================================================================
#define BLINK_COUNT 20      // Number of state changes (5 full blinks)
#define BLINK_PERIOD_MS 150 // 200ms per state change (100ms on, 100ms off)

// ============================================================================
// Shared State Variables (volatile for ISR/task safety)
// ============================================================================

// Button press detection flag (set by Task 1, consumed by Task 2)
extern volatile bool g_buttonPressed;

// Global push counter (incremented by Task 2, read/reset by Task 3)
extern volatile uint16_t g_pushCount;

// LED2 blink state machine control
extern volatile bool g_blinkSequenceActive;
extern volatile uint8_t g_blinkState;
extern volatile uint8_t g_blinkCounter;

// ============================================================================
// Task Function Declarations
// ============================================================================

// Task 1: Button Detector
// - Scans physical button input with software debouncing
// - Toggles LED1 on valid press detection
// - Sets g_buttonPressed flag for Task 2
void taskButtonDetector(void);

// Task 2: Press Counter & Visualizer
// - Monitors g_buttonPressed from Task 1
// - Increments g_pushCount on detection
// - Triggers rapid blink sequence on LED2 (10 state changes at 200ms)
void taskPressCounter(void);

// Task 3: Status Monitor
// - Executes every 10 seconds
// - Reads and transmits g_pushCount via UART
// - Resets counter to zero
void taskStatusMonitor(void);

// ============================================================================
// Initialization Function
// ============================================================================

// Initialize all tasks and hardware
void tasksInit(void);

#endif // TASKS_H
