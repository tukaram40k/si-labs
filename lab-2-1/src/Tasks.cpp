#include "Tasks.h"
#include "Scheduler.h"
#include <stdio.h>

// Shared state variables
volatile uint32_t pushCount = 0;
volatile bool triggerRapidBlink = false;

// Task 1 (Button Detector) state variables
static bool lastButtonState = HIGH;      // Previous button reading
static uint32_t lastDebounceTime = 0;    // Last debounce timestamp
static bool led1State = false;           // Current LED1 state
static bool buttonPressDetected = false; // Debounced press detection flag

// Task 2 (Press Counter & Visualizer) state variables
static bool rapidBlinkActive = false;    // Rapid blink sequence active flag
static uint8_t blinkCounter = 0;         // Blink state counter
static uint32_t lastBlinkTime = 0;       // Last blink toggle time
static bool led2State = false;           // Current LED2 state

// Task 3 (Status Monitor) state variables
static uint32_t lastStatusTime = 0;      // Last status report time

void tasksInit(void) {
    // Configure button pin with internal pull-up
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    // Configure LED pins as outputs
    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);
    pinMode(LED3_PIN, OUTPUT);

    // Initialize LEDs to OFF state
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, LOW);
    digitalWrite(LED3_PIN, LOW);

    // Initialize state variables
    lastButtonState = digitalRead(BUTTON_PIN);
    lastDebounceTime = Scheduler::getTickCount();
    led1State = false;
    rapidBlinkActive = false;
    blinkCounter = 0;
    lastBlinkTime = 0;
    led2State = false;
    lastStatusTime = 0;
}

// Task 1: Button Detector
// - Scans button input periodically
// - Implements software debouncing
// - Toggles LED1 on valid press
// - Must respond within 100ms
void task1_ButtonDetector(void) {
    uint32_t currentTime = Scheduler::getTickCount();
    bool reading = digitalRead(BUTTON_PIN);

    // Reset debounce timer on state change
    if (reading != lastButtonState) {
        lastDebounceTime = currentTime;
    }
    lastButtonState = reading;

    // Check if debounce time has passed and button is pressed
    if ((currentTime - lastDebounceTime) > DEBOUNCE_DELAY_MS) {
        // Detect falling edge (button press with pull-up)
        if (reading == LOW && !buttonPressDetected) {
            buttonPressDetected = true;
            
            // Toggle LED1
            led1State = !led1State;
            digitalWrite(LED1_PIN, led1State ? HIGH : LOW);
            
            // Trigger rapid blink sequence
            triggerRapidBlink = true;
        }
        // Button released
        else if (reading == HIGH && buttonPressDetected) {
            buttonPressDetected = false;
        }
    }
}

// Task 2: Press Counter & Visualizer
// - Monitors button press flag from Task 1
// - Increments push_count and triggers rapid blink on LED2
// - Rapid blink: 10 state changes (5 full blinks) at 20ms frequency
void task2_PressCounterVisualizer(void) {
    uint32_t currentTime = Scheduler::getTickCount();

    // Check if rapid blink sequence should start
    if (triggerRapidBlink && !rapidBlinkActive) {
        rapidBlinkActive = true;
        blinkCounter = 0;
        lastBlinkTime = currentTime;
        triggerRapidBlink = false;

        // Increment global push counter
        pushCount++;

        // Turn on LED2 to start the sequence
        led2State = true;
        digitalWrite(LED2_PIN, HIGH);
        
        printf("Blink sequence started. Count: %lu\n", pushCount);
    }

    // Execute rapid blink sequence
    if (rapidBlinkActive) {
        // Check if it's time to toggle LED2
        if ((currentTime - lastBlinkTime) >= RAPID_BLINK_PERIOD_MS) {
            lastBlinkTime = currentTime;
            blinkCounter++;

            // Toggle LED2 state
            led2State = !led2State;
            digitalWrite(LED2_PIN, led2State ? HIGH : LOW);

            printf("Blink toggle %d, LED2=%s\n", blinkCounter, led2State ? "ON" : "OFF");

            // Check if sequence is complete (10 state changes = 5 full blinks)
            if (blinkCounter >= RAPID_BLINK_COUNT) {
                rapidBlinkActive = false;
                // Ensure LED2 is OFF at end of sequence
                digitalWrite(LED2_PIN, LOW);
                led2State = false;
                printf("Blink sequence complete.\n");
            }
        }
    }
}

// Task 3: Status Monitor
// - Executes every 10 seconds
// - Reads push_count, transmits via UART, resets counter
void task3_StatusMonitor(void) {
    uint32_t currentTime = Scheduler::getTickCount();

    // Check if 10 seconds have passed
    if ((currentTime - lastStatusTime) >= TASK3_PERIOD_MS) {
        lastStatusTime = currentTime;

        // Read current push count (atomic access)
        noInterrupts();
        uint32_t count = pushCount;
        pushCount = 0;
        interrupts();

        // Transmit via UART
        printf("Total presses: %lu\n", count);

        // Toggle LED3 as heartbeat indicator
        static bool led3State = false;
        led3State = !led3State;
        digitalWrite(LED3_PIN, led3State ? HIGH : LOW);
    }
}
