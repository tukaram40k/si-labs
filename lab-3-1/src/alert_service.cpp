#include "alert_service.h"

void alert_evaluator_init(alert_evaluator_t *e) {
    e->state = ALERT_OFF;
    e->raw_condition = false;
    e->persist_start_ms = 0;
    e->persisting = false;
}

alert_state_t alert_evaluator_update(alert_evaluator_t *e, float temperature, uint32_t now_ms) {
    // Determine raw threshold condition with hysteresis
    bool new_condition;
    if (e->state == ALERT_OFF) {
        // Alert triggers when temperature exceeds upper threshold
        new_condition = (temperature > ALERT_HIGH);
    } else {
        // Alert clears when temperature drops below lower threshold
        new_condition = !(temperature < ALERT_LOW);
    }

    // Debouncing: condition must persist for ALERT_DEBOUNCE_MS
    if (new_condition != e->raw_condition) {
        // Condition changed — reset persistence timer
        e->raw_condition = new_condition;
        e->persist_start_ms = now_ms;
        e->persisting = true;
    } else if (e->persisting) {
        // Same condition — check if enough time has elapsed
        if ((now_ms - e->persist_start_ms) >= ALERT_DEBOUNCE_MS) {
            // Condition persisted long enough — update state
            e->state = new_condition ? ALERT_ON : ALERT_OFF;
            e->persisting = false;
        }
    }

    return e->state;
}