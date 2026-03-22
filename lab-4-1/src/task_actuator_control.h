#ifndef TASK_ACTUATOR_CONTROL_H
#define TASK_ACTUATOR_CONTROL_H

#include <Arduino.h>

class RemoteDriver;

void task_actuator_control_setup();
void task_actuator_control_tick();

void task_actuator_control_bind_remote(RemoteDriver* remote);

#endif
