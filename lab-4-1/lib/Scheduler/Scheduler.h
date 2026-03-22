#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Arduino.h>
#include <stdint.h>

typedef void (*TaskFn)();

struct TaskItem {
  TaskFn fn;
  uint32_t periodMs;
  uint32_t nextRunAt;
};

class Scheduler {
public:
  Scheduler(TaskItem* tasks, size_t taskCount);

  void start(uint32_t nowMs);
  void tick(uint32_t nowMs);

private:
  TaskItem* m_tasks;
  size_t m_taskCount;
};

#endif
