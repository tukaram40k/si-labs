#ifndef TASK_REPORTING_H
#define TASK_REPORTING_H

#include <Arduino.h>
#include "../lib/Actuator/Actuator.h"

void task_reporting_setup(Actuator* actuator);
void task_reporting_tick();

#endif
