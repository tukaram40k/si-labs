#include "alert_service.h"

void alert_evaluator_init(alert_evaluator_t *e)
{
  e->state = ALERT_OFF;
  e->raw_condition = false;
  e->persist_start_ms = 0;
  e->persisting = false;
}

alert_state_t alert_evaluator_update(alert_evaluator_t *e, float temperature, uint32_t now_ms)
{
  bool new_condition;
  if (e->state == ALERT_OFF)
  {
    new_condition = (temperature > ALERT_HIGH);
  }
  else
  {
    new_condition = !(temperature < ALERT_LOW);
  }

  if (new_condition != e->raw_condition)
  {
    e->raw_condition = new_condition;
    e->persist_start_ms = now_ms;
    e->persisting = true;
  }
  else if (e->persisting)
  {
    if ((now_ms - e->persist_start_ms) >= ALERT_DEBOUNCE_MS)
    {
      e->state = new_condition ? ALERT_ON : ALERT_OFF;
      e->persisting = false;
    }
  }

  return e->state;
}