#include "Tasks.h"
#include "Scheduler.h"
#include <Arduino.h>
#include <stdio.h>

// ============================================================================
// Shared State Variables
// ============================================================================

volatile bool g_buttonPressed = false;
volatile uint16_t g_pushCount = 0;
volatile bool g_blinkSequenceActive = false;
volatile uint8_t g_blinkState = 0;
volatile uint8_t g_blinkCounter = 0;

// ============================================================================
// Task 1: Button Detector - Static Variables (State Machine)
// ============================================================================

static uint8_t s_debounceCounter = 0;
static bool s_lastButtonState = false;
static bool s_buttonDetected = false;

// ============================================================================
// Task 2: Press Counter - Static Variables (State Machine)
// ============================================================================

static uint8_t s_blinkIndex = 0;
static uint32_t s_lastBlinkTime = 0;

// ============================================================================
// Task 3: Status Monitor - Static Variables (State Machine)
// ============================================================================

static bool s_statusPending = false;

// ============================================================================
// Task 1: Button Detector Implementation
// ============================================================================
/**
 * Task 1: Button Detector
 * 
 * Period: 10ms
 * Offset: 0ms
 * 
 * Implements software debouncing for the physical button.
 * When a valid press is detected:
 * - Toggles LED1
 * - Sets g_buttonPressed flag for Task 2
 * 
 * Latency requirement: < 100ms (achieved with 10ms polling + 50ms debounce)
 */
void taskButtonDetector(void) {
    // Read current button state (active low due to pull-up configuration)
    bool currentButtonState = (digitalRead(PIN_BUTTON) == LOW);
    
    // Debounce logic: count consecutive identical readings
    if (currentButtonState != s_lastButtonState) {
        // State changed, reset debounce counter
        s_debounceCounter = 0;
        s_lastButtonState = currentButtonState;
    } else {
        // State stable, increment counter
        if (s_debounceCounter < DEBOUNCE_STABLE_COUNT) {
            s_debounceCounter++;
        }
        
        // Check if we have a stable button press
        if (s_debounceCounter == DEBOUNCE_STABLE_COUNT && currentButtonState && !s_buttonDetected) {
            // Valid button press detected
            s_buttonDetected = true;
            
            // Toggle LED1
            digitalWrite(PIN_LED1, !digitalRead(PIN_LED1));
            
            // Signal Task 2 that a button was pressed
            g_buttonPressed = true;
        }
    }
    
    // Reset detection flag when button is released
    if (!currentButtonState && s_buttonDetected) {
        s_buttonDetected = false;
    }
}

// ============================================================================
// Task 2: Press Counter & Visualizer Implementation
// ============================================================================
/**
 * Task 2: Press Counter & Visualizer
 *
 * Period: 50ms
 * Offset: 5ms
 *
 * Monitors g_buttonPressed from Task 1.
 * Upon detection:
 * - Increments global push_count
 * - Triggers rapid blink sequence on LED2
 *
 * Blink sequence: 10 state changes (5 full blinks) at 200ms frequency
 */
void taskPressCounter(void) {
    // Get current system time
    uint32_t currentTime = Scheduler::getInstance().getTickCount();

    // Check for new button press from Task 1
    if (g_buttonPressed) {
        // Consume the button press flag
        g_buttonPressed = false;

        // Increment push counter
        g_pushCount++;

        // Start blink sequence
        g_blinkSequenceActive = true;
        s_blinkIndex = 0;
        s_lastBlinkTime = currentTime;

        // Turn on LED2 to start the sequence
        digitalWrite(PIN_LED2, HIGH);
        g_blinkState = HIGH;
    }

    // Handle blink sequence state machine (only if sequence is active)
    if (g_blinkSequenceActive) {
        // Check if 200ms has elapsed since last state change
        if (currentTime - s_lastBlinkTime >= BLINK_PERIOD_MS) {
            s_lastBlinkTime = currentTime;
            s_blinkIndex++;

            // Check if we've completed all 10 state changes
            if (s_blinkIndex >= BLINK_COUNT) {
                // Sequence complete: 10 state changes = 5 full blinks
                // Total time: 10 * 200ms = 2 seconds
                g_blinkSequenceActive = false;
                s_blinkIndex = 0;

                // Ensure LED2 is off at end of sequence
                digitalWrite(PIN_LED2, LOW);
                g_blinkState = LOW;
            } else {
                // Toggle LED2 state
                g_blinkState = !g_blinkState;
                digitalWrite(PIN_LED2, g_blinkState);
            }
        }
    }
}

// ============================================================================
// Task 3: Status Monitor Implementation
// ============================================================================
/**
 * Task 3: Status Monitor
 * 
 * Period: 10000ms (10 seconds)
 * Offset: 1000ms
 * 
 * Executes strictly every 10 seconds.
 * - Reads current push_count
 * - Transmits value via UART/Serial
 * - Resets counter to zero
 */
void taskStatusMonitor(void) {
    // Read current count (atomic access)
    noInterrupts();
    uint16_t currentCount = g_pushCount;
    interrupts();
    
    // Transmit via UART
    printf("Total presses: %u\n", currentCount);
    
    // Reset counter
    g_pushCount = 0;
    
    // Toggle LED3 as heartbeat indicator
    digitalWrite(PIN_LED3, !digitalRead(PIN_LED3));
}

// ============================================================================
// Initialization Function
// ============================================================================
/**
 * Initialize all tasks and hardware pins
 */
void tasksInit(void) {
    // Configure LED pins as outputs
    pinMode(PIN_LED1, OUTPUT);
    pinMode(PIN_LED2, OUTPUT);
    pinMode(PIN_LED3, OUTPUT);
    
    // Configure button pin as input with internal pull-up
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    
    // Initialize all LEDs to OFF
    digitalWrite(PIN_LED1, LOW);
    digitalWrite(PIN_LED2, LOW);
    digitalWrite(PIN_LED3, LOW);
    
    // Initialize shared state variables
    g_buttonPressed = false;
    g_pushCount = 0;
    g_blinkSequenceActive = false;
    g_blinkState = 0;
    g_blinkCounter = 0;
    
    // Initialize task state machine variables
    s_debounceCounter = 0;
    s_lastButtonState = false;
    s_buttonDetected = false;
    s_blinkIndex = 0;
    s_lastBlinkTime = 0;
    s_statusPending = false;
    
    // Register tasks with scheduler
    Scheduler& scheduler = Scheduler::getInstance();
    
    scheduler.addTask(taskButtonDetector, TASK1_PERIOD_MS, TASK1_OFFSET_MS);
    scheduler.addTask(taskPressCounter, TASK2_PERIOD_MS, TASK2_OFFSET_MS);
    scheduler.addTask(taskStatusMonitor, TASK3_PERIOD_MS, TASK3_OFFSET_MS);
}
