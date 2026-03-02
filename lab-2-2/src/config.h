#ifndef CONFIG_H
#define CONFIG_H

// GPIO Pin Definitions (per diagram.json)
#define BUTTON_PIN      14
#define GREEN_LED_PIN   16
#define RED_LED_PIN     17
#define YELLOW_LED_PIN  18

// Timing Configuration
#define SHORT_PRESS_THRESHOLD_MS    500
#define TASK_MEASURE_PERIOD_MS      10
#define YELLOW_BLINK_DELAY_MS       100
#define INDICATION_DURATION_MS      150
#define TASK_REPORT_PERIOD_MS       10000

// Task Configuration
#define TASK_MEASURE_STACK_SIZE     8192
#define TASK_MEASURE_PRIORITY       2

#define TASK_STATS_STACK_SIZE       8192
#define TASK_STATS_PRIORITY         2

#define TASK_REPORT_STACK_SIZE      8192
#define TASK_REPORT_PRIORITY        2

#endif // CONFIG_H
