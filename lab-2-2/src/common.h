#ifndef COMMON_H
#define COMMON_H

#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <stdint.h>
#include <stdbool.h>

// Hardware pin definitions
#define PIN_LED_GREEN 12    // Green LED - SHORT press indicator
#define PIN_LED_RED 11      // Red LED - LONG press indicator
#define PIN_LED_YELLOW 10   // Yellow LED - Blink feedback
#define PIN_BUTTON 2        // Button input

// Timing configuration
#define BUTTON_POLL_PERIOD_MS 10      // Button polling period
#define DEBOUNCE_STABLE_MS 50         // Debounce time
#define DEBOUNCE_STABLE_COUNT (DEBOUNCE_STABLE_MS / BUTTON_POLL_PERIOD_MS)

#define PRESS_DURATION_THRESHOLD_MS 500  // Short vs Long threshold

#define GREEN_RED_INDICATION_MS 150   // Green/Red LED on duration
#define YELLOW_BLINK_PERIOD_MS 80     // Yellow blink period
#define SHORT_BLINK_COUNT 5           // Short press blinks
#define LONG_BLINK_COUNT 10           // Long press blinks

#define REPORT_PERIOD_MS 10000        // Statistics report period

// Press type enumeration
typedef enum {
    PRESS_NONE = 0,
    PRESS_SHORT,
    PRESS_LONG
} PressType;

// Statistics structure
typedef struct {
    uint32_t total_presses;
    uint32_t short_presses;
    uint32_t long_presses;
    uint32_t sum_short_duration;
    uint32_t sum_long_duration;
} Statistics;

// Shared state structure
typedef struct {
    // Synchronization primitives
    SemaphoreHandle_t press_semaphore;  // Binary semaphore: Task1 -> Task2
    SemaphoreHandle_t stats_mutex;      // Mutex for statistics protection

    // Shared data (protected by stats_mutex)
    Statistics stats;
    uint32_t last_duration_ms;
    PressType last_press_type;

    // LED state
    bool green_led_on;
    bool red_led_on;
} SharedState;

// Global shared state
extern SharedState g_shared;

// Function prototypes
void sharedStateInit(void);

#endif // COMMON_H
