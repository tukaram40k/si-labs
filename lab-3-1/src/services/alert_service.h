#ifndef ALERT_SERVICE_H
#define ALERT_SERVICE_H

#include "shared_data.h"
#include "config.h"
#include <stdint.h>

typedef struct
{
  alert_state_t state;
  bool raw_condition;
  uint32_t persist_start_ms;
  bool persisting;
} alert_evaluator_t;

void alert_evaluator_init(alert_evaluator_t *e);
alert_state_t alert_evaluator_update(alert_evaluator_t *e, float temperature, uint32_t now_ms);

#endif // ALERT_SERVICE_H