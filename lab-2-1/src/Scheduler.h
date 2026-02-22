#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Arduino.h>
#include <stdint.h>

// Maximum number of tasks in the scheduler
#define MAX_TASKS 10

// System tick period in milliseconds
#define SYSTEM_TICK_MS 1

// Task function pointer type
typedef void (*TaskFunction)(void);

// Task Control Block structure
typedef struct {
    TaskFunction function;      // Pointer to task function
    uint32_t period;            // Task period in milliseconds
    uint32_t offset;            // Initial offset/delay before first execution
    uint32_t nextRunTime;       // Next scheduled run time (in system ticks)
    bool enabled;               // Task enabled flag
} TaskControlBlock;

// Scheduler class for non-preemptive multitasking
class Scheduler {
public:
    // Initialize the scheduler
    static void init();

    // Start the scheduler (enables timer interrupt)
    static void start();

    // Stop the scheduler (disables timer interrupt)
    static void stop();

    // Add a task to the scheduler
    // Returns task index on success, -1 on failure
    static int addTask(TaskFunction function, uint32_t period, uint32_t offset = 0);

    // Remove a task from the scheduler
    static void removeTask(int taskIndex);

    // Enable/disable a task
    static void enableTask(int taskIndex, bool enabled);

    // Run the scheduler (call from main loop)
    // Executes all tasks that are due
    static void run();

    // Get current system tick count
    static uint32_t getTickCount();

    // Get the number of registered tasks
    static int getTaskCount();

    // Internal tick increment (called by ISR)
    static void tick();

private:
    static TaskControlBlock tasks[MAX_TASKS];
    static int taskCount;
    static volatile uint32_t systemTick;

    // Timer1 interrupt service routine setup
    static void setupTimer1();
};

#endif // SCHEDULER_H
