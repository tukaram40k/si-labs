#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Arduino.h>
#include <stdint.h>

#define SYSTEM_TICK_MS 1       // system tick period
#define MAX_TASKS 10           // max tasks

typedef void (*TaskFunction)(void);

// task control block
typedef struct
{
  TaskFunction function;   // task function pointer
  uint32_t period;         // period in ms
  uint32_t offset;         // initial offset in ms
  uint32_t nextRunTime;    // next run time (ticks)
  bool enabled;            // enabled flag
} TaskControlBlock;

// non-preemptive scheduler
class Scheduler
{
public:
  static Scheduler &getInstance();  // singleton accessor

  void begin();  // init scheduler, start system tick

  bool addTask(TaskFunction function, uint32_t period, uint32_t offset = 0);  // add task
  bool removeTask(TaskFunction function);  // remove task
  void setTaskEnabled(TaskFunction function, bool enabled);  // enable/disable task

  void run();  // run scheduler (call in loop)
  uint32_t getTickCount() const;  // get current tick count
  uint8_t getTaskCount() const;   // get registered task count

  void incrementTickCount();  // increment tick (called from isr)

private:
  Scheduler();  // private constructor

  Scheduler(const Scheduler &) = delete;
  Scheduler &operator=(const Scheduler &) = delete;

  volatile uint32_t m_tickCount;
  TaskControlBlock m_tasks[MAX_TASKS];
  uint8_t m_taskCount;

  static void timerISR();  // timer isr
};

extern volatile uint32_t g_systemTick;  // global tick counter

#endif // SCHEDULER_H
