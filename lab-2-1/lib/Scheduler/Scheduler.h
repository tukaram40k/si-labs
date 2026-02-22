#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Arduino.h>
#include <stdint.h>

// System tick period in milliseconds
#define SYSTEM_TICK_MS 1

// Maximum number of tasks in the scheduler
#define MAX_TASKS 10

// Task function pointer type
typedef void (*TaskFunction)(void);

// Task Control Block structure
typedef struct
{
  TaskFunction function; // Pointer to task function
  uint32_t period;       // Task period in milliseconds
  uint32_t offset;       // Initial offset in milliseconds
  uint32_t nextRunTime;  // Next scheduled run time (in system ticks)
  bool enabled;          // Task enabled flag
} TaskControlBlock;

// Scheduler class for non-preemptive multitasking
class Scheduler
{
public:
  // Get singleton instance
  static Scheduler &getInstance();

  // Initialize the scheduler and start system tick timer
  void begin();

  // Add a task to the scheduler
  bool addTask(TaskFunction function, uint32_t period, uint32_t offset = 0);

  // Remove a task from the scheduler
  bool removeTask(TaskFunction function);

  // Enable or disable a task
  void setTaskEnabled(TaskFunction function, bool enabled);

  // Run the scheduler (call in main loop)
  void run();

  // Get current system tick count
  uint32_t getTickCount() const;

  // Get number of registered tasks
  uint8_t getTaskCount() const;

  // Increment tick count (called from ISR)
  void incrementTickCount();

private:
  // Private constructor for singleton
  Scheduler();

  // Disable copy constructor and assignment operator
  Scheduler(const Scheduler &) = delete;
  Scheduler &operator=(const Scheduler &) = delete;

  // System tick counter
  volatile uint32_t m_tickCount;

  // Task list
  TaskControlBlock m_tasks[MAX_TASKS];

  // Number of registered tasks
  uint8_t m_taskCount;

  // Timer interrupt service routine
  static void timerISR();
};

// Global system tick counter (accessible for debugging)
extern volatile uint32_t g_systemTick;

#endif // SCHEDULER_H
