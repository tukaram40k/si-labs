#ifndef ALERT_SERVICE_H
#define ALERT_SERVICE_H

#include "shared_data.h"
#include "config.h"
#include <stdint.h>

// ============================================================
// Alert evaluator with hysteresis and debouncing
// ============================================================
typedef struct
{
  alert_state_t state;       // Current alert state
  bool raw_condition;        // Raw threshold result (before debounce)
  uint32_t persist_start_ms; // When the raw condition started
  bool persisting;           // Whether we're counting persistence
} alert_evaluator_t;

void alert_evaluator_init(alert_evaluator_t *e);
alert_state_t alert_evaluator_update(alert_evaluator_t *e, float temperature, uint32_t now_ms);

#endif // ALERT_SERVICE_H