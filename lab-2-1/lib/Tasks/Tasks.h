#ifndef TASKS_H
#define TASKS_H

#include <stdint.h>
#include <stdbool.h>

// hardware pin definitions
#define PIN_LED1 12  // red led - status/toggle
#define PIN_LED2 11  // green led - rapid blink
#define PIN_LED3 10  // yellow led - heartbeat
#define PIN_BUTTON 2 // button input

// task timing configuration
// task 1: button detector (10ms)
#define TASK1_PERIOD_MS 10
#define TASK1_OFFSET_MS 0

// task 2: press counter (50ms)
#define TASK2_PERIOD_MS 50
#define TASK2_OFFSET_MS 5

// task 3: status monitor (10s)
#define TASK3_PERIOD_MS 10000
#define TASK3_OFFSET_MS 1000

// debounce configuration
#define DEBOUNCE_DELAY_MS 50 // debounce delay
#define DEBOUNCE_STABLE_COUNT (DEBOUNCE_DELAY_MS / TASK1_PERIOD_MS)

// blink sequence configuration
#define BLINK_COUNT 10      // state changes
#define BLINK_PERIOD_MS 150 // 150ms per state change

// shared state variables (volatile for isr/task safety)
extern volatile bool g_buttonPressed;    // set by task 1, consumed by task 2
extern volatile uint16_t g_pushCount;    // incremented by task 2, reset by task 3
extern volatile bool g_blinkSequenceActive;
extern volatile uint8_t g_blinkState;
extern volatile uint8_t g_blinkCounter;

// task function declarations
void taskButtonDetector(void);   // scan button, toggle led1, set flag
void taskPressCounter(void);     // count presses, trigger led2 blink
void taskStatusMonitor(void);    // log count every 10s, reset

// initialization
void tasksInit(void);

#endif // TASKS_H
