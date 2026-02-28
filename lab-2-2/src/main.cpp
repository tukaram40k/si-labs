#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>

// Hardware pin definitions
#define PIN_LED_GREEN 11
#define PIN_LED_RED 12
#define PIN_LED_YELLOW 10
#define PIN_BUTTON 2

// Press type enumeration
typedef enum {
    PRESS_NONE = 0,
    PRESS_SHORT,
    PRESS_LONG
} PressType;

// Statistics structure
typedef struct {
    uint32_t totalPresses;
    uint32_t shortPressCount;
    uint32_t longPressCount;
    uint32_t sumShortDuration;
    uint32_t sumLongDuration;
} Statistics;

// Shared variables
static SemaphoreHandle_t xPressSemaphore = NULL;
static SemaphoreHandle_t xStatsMutex = NULL;
static volatile PressType xLastPressType = PRESS_NONE;
static volatile uint32_t ulLastPressDuration = 0;
static Statistics xStats = {0, 0, 0, 0, 0};

// Forward declarations
void vTaskMeasure(void *pvParameters);
void vTaskStats(void *pvParameters);
void vTaskReport(void *pvParameters);

// FreeRTOS hooks
extern "C" {
    void vApplicationIdleHook(void) {}
    void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) { for (;;); }
    void vApplicationMallocFailedHook(void) { for (;;); }
}

void setup() {
    Serial.begin(9600);
    while (!Serial);
    
    Serial.println("=== FreeRTOS System Starting ===");
    delay(10);

    // Initialize pins directly
    pinMode(PIN_LED_GREEN, OUTPUT);
    pinMode(PIN_LED_RED, OUTPUT);
    pinMode(PIN_LED_YELLOW, OUTPUT);
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    digitalWrite(PIN_LED_GREEN, LOW);
    digitalWrite(PIN_LED_RED, LOW);
    digitalWrite(PIN_LED_YELLOW, LOW);

    Serial.println("Hardware initialized");
    delay(10);

    // Create semaphores
    xPressSemaphore = xSemaphoreCreateBinary();
    xStatsMutex = xSemaphoreCreateMutex();

    if (xPressSemaphore == NULL || xStatsMutex == NULL) {
        Serial.println("Semaphore creation failed!");
        for (;;);
    }
    Serial.println("Semaphores created");
    delay(10);

    // Initialize press semaphore
    xSemaphoreGive(xPressSemaphore);
    xSemaphoreTake(xPressSemaphore, portMAX_DELAY);

    Serial.println("Creating tasks...");
    delay(10);

    // Create tasks
    BaseType_t xResult = xTaskCreate(vTaskMeasure, "Measure", 150, NULL, 3, NULL);
    Serial.print("Task Measure: ");
    Serial.println(xResult == pdPASS ? "OK" : "FAIL");
    delay(10);

    xResult = xTaskCreate(vTaskStats, "Stats", 150, NULL, 2, NULL);
    Serial.print("Task Stats: ");
    Serial.println(xResult == pdPASS ? "OK" : "FAIL");
    delay(10);

    xResult = xTaskCreate(vTaskReport, "Report", 150, NULL, 1, NULL);
    Serial.print("Task Report: ");
    Serial.println(xResult == pdPASS ? "OK" : "FAIL");
    delay(10);

    Serial.println("Starting scheduler...");
    delay(10);

    vTaskStartScheduler();

    Serial.println("Scheduler failed!");
    for (;;);
}

void loop() {}

// Task 1: Button Detection
void vTaskMeasure(void *pvParameters) {
    (void)pvParameters;

    uint32_t ulPressStartTime = 0;
    uint32_t ulDuration = 0;
    PressType xPressType = PRESS_NONE;
    bool xWaitingForRelease = false;
    bool xLastButtonState = false;
    uint32_t ulLastDebounceTime = 0;
    const uint32_t ulDebounceDelay = 50;

    for (;;) {
        bool xReading = digitalRead(PIN_BUTTON) == LOW;

        if (xReading != xLastButtonState) {
            ulLastDebounceTime = xTaskGetTickCount() * portTICK_PERIOD_MS;
        }
        xLastButtonState = xReading;

        if ((xTaskGetTickCount() * portTICK_PERIOD_MS - ulLastDebounceTime) > ulDebounceDelay) {
            if (xReading && !xWaitingForRelease) {
                xWaitingForRelease = true;
                ulPressStartTime = xTaskGetTickCount() * portTICK_PERIOD_MS;
            }
            
            if (!xReading && xWaitingForRelease) {
                ulDuration = (xTaskGetTickCount() * portTICK_PERIOD_MS) - ulPressStartTime;

                if (ulDuration < 500) {
                    xPressType = PRESS_SHORT;
                    digitalWrite(PIN_LED_GREEN, HIGH);
                } else {
                    xPressType = PRESS_LONG;
                    digitalWrite(PIN_LED_RED, HIGH);
                }

                if (xSemaphoreTake(xStatsMutex, portMAX_DELAY) == pdTRUE) {
                    xLastPressType = xPressType;
                    ulLastPressDuration = ulDuration;
                    xSemaphoreGive(xStatsMutex);
                }

                vTaskDelay(150 / portTICK_PERIOD_MS);
                digitalWrite(PIN_LED_GREEN, LOW);
                digitalWrite(PIN_LED_RED, LOW);

                xSemaphoreGive(xPressSemaphore);
                xWaitingForRelease = false;
            }
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

// Task 2: Statistics & Yellow LED
void vTaskStats(void *pvParameters) {
    (void)pvParameters;

    for (;;) {
        if (xSemaphoreTake(xPressSemaphore, portMAX_DELAY) == pdTRUE) {
            PressType xPressType = PRESS_NONE;
            uint32_t ulDuration = 0;

            if (xSemaphoreTake(xStatsMutex, portMAX_DELAY) == pdTRUE) {
                xPressType = xLastPressType;
                ulDuration = ulLastPressDuration;
                xSemaphoreGive(xStatsMutex);
            }

            if (xSemaphoreTake(xStatsMutex, portMAX_DELAY) == pdTRUE) {
                xStats.totalPresses++;
                if (xPressType == PRESS_SHORT) {
                    xStats.shortPressCount++;
                    xStats.sumShortDuration += ulDuration;
                } else {
                    xStats.longPressCount++;
                    xStats.sumLongDuration += ulDuration;
                }
                xSemaphoreGive(xStatsMutex);
            }

            uint8_t uxBlinkCount = (xPressType == PRESS_SHORT) ? 5 : 10;
            for (uint8_t i = 0; i < uxBlinkCount; i++) {
                digitalWrite(PIN_LED_YELLOW, HIGH);
                vTaskDelay(80 / portTICK_PERIOD_MS);
                digitalWrite(PIN_LED_YELLOW, LOW);
                vTaskDelay(80 / portTICK_PERIOD_MS);
            }
        }
    }
}

// Task 3: Periodic Reporting
void vTaskReport(void *pvParameters) {
    (void)pvParameters;

    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xReportPeriod = 10000 / portTICK_PERIOD_MS;

    for (;;) {
        vTaskDelayUntil(&xLastWakeTime, xReportPeriod);
        digitalWrite(PIN_LED_YELLOW, !digitalRead(PIN_LED_YELLOW));
    }
}
