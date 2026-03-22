#include "task_signal_conditioning.h"

#include <Arduino.h>

// provided by task_actuator_control.cpp
bool control_consume_pending_request(bool& outOn);

static ConditioningConfig g_cfg;
static Actuator* g_actuator = nullptr;

static bool g_candidateOn = false;
static bool g_haveCandidate = false;
static uint32_t g_candidateSinceMs = 0;
static uint8_t g_stableCount = 0;

static bool g_hasUnappliedRequest = false;

bool conditioning_has_unapplied_request() {
  return g_hasUnappliedRequest;
}

void task_signal_conditioning_setup(const ConditioningConfig& cfg, Actuator* actuator) {
  g_cfg = cfg;
  g_actuator = actuator;

  g_haveCandidate = false;
  g_stableCount = 0;
  g_hasUnappliedRequest = false;
}

void task_signal_conditioning_tick() {
  if (!g_actuator) {
    return;
  }

  const uint32_t now = millis();

  bool requestedOn = false;
  const bool hasReq = control_consume_pending_request(requestedOn);

  if (hasReq) {
    // Saturation: requestedOn is already a bool.

    if (!g_haveCandidate || requestedOn != g_candidateOn) {
      // New candidate (edge)
      g_haveCandidate = true;
      g_candidateOn = requestedOn;
      g_candidateSinceMs = now;
      g_stableCount = 1;
      g_hasUnappliedRequest = true;
    } else {
      // Same request repeated
      if (g_stableCount < 255) {
        g_stableCount++;
      }
    }
  }

  // If we have a candidate, finish debounce based on elapsed time.
  if (!g_haveCandidate) {
    return;
  }

  const bool timeOk = (uint32_t)(now - g_candidateSinceMs) >= g_cfg.debounceMs;
  const bool samplesOk = g_stableCount >= g_cfg.requiredStableSamples;
  if (!timeOk || !samplesOk) {
    return;
  }

  // Persistent validation: apply only if it changes state.
  const bool desiredOn = g_candidateOn;
  const bool currentOn = (g_actuator->getState() == ActuatorState::ON);

  if (desiredOn != currentOn) {
    g_actuator->setOn(desiredOn);
  }

  g_haveCandidate = false;
  g_hasUnappliedRequest = false;
}
