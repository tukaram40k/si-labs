#ifndef TASK_SIGNAL_CONDITIONING_H
#define TASK_SIGNAL_CONDITIONING_H

#include <Arduino.h>
#include "../../lib/Actuator/Actuator.h"

struct ConditioningConfig {
  uint16_t debounceMs;
  uint8_t requiredStableSamples;
};

void task_signal_conditioning_setup(const ConditioningConfig& cfg, Actuator* actuator);
void task_signal_conditioning_tick();

// For reporting.
bool conditioning_has_unapplied_request();

#endif
