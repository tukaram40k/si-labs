#include "task_conditioning.h"

#include "ButtonDriver.h"
#include "Dht11Driver.h"

namespace
{
  TaskConditioning::Config g_cfg{3, 9, 8, true, 30, 55, 30, 90, 1, 2.0f, 0.2f, 0.8f, 0.0f, 100.0f, 30, 2000, 2000, 2000};
  TaskConditioning::State g_state{};

  ButtonDriver g_upButton;
  ButtonDriver g_downButton;
  Dht11Driver g_dht;

  uint32_t g_lastButtonsMs = 0;
  uint32_t g_lastSampleMs = 0;
  uint32_t g_lastControlMs = 0;

  float g_integralTerm = 0.0f;
  float g_prevError = 0.0f;
  bool g_hasPrevError = false;

  int clampInt(int v, int lo, int hi)
  {
    if (v < lo)
      return lo;
    if (v > hi)
      return hi;
    return v;
  }

  float clampFloat(float v, float lo, float hi)
  {
    if (v < lo)
      return lo;
    if (v > hi)
      return hi;
    return v;
  }
}

namespace TaskConditioning
{
  void setup(const Config &cfg)
  {
    g_cfg = cfg;

    g_upButton.setup(ButtonDriver::Config{
        .pin = g_cfg.upButtonPin,
        .activeLow = g_cfg.buttonsActiveLow,
        .debounceMs = g_cfg.buttonDebounceMs,
    });
    g_downButton.setup(ButtonDriver::Config{
        .pin = g_cfg.downButtonPin,
        .activeLow = g_cfg.buttonsActiveLow,
        .debounceMs = g_cfg.buttonDebounceMs,
    });
    g_dht.setup(Dht11Driver::Config{.dataPin = g_cfg.dhtPin});

    g_state = {};
    g_state.humidityPct = 0.0f;
    g_state.humidityValid = false;
    g_state.setPointPct = clampInt(g_cfg.setPointInitPct, g_cfg.setPointMinPct, g_cfg.setPointMaxPct);
    g_state.controlOutputPct = 0.0f;
    g_state.controlDutyPct = 0.0f;
    g_state.actuatorRequestOn = false;

    g_integralTerm = 0.0f;
    g_prevError = 0.0f;
    g_hasPrevError = false;

    const uint32_t now = millis();
    g_lastButtonsMs = now - g_cfg.buttonPeriodMs;
    g_lastSampleMs = now - g_cfg.samplePeriodMs;
    g_lastControlMs = now - g_cfg.controlPeriodMs;
  }

  void tick()
  {
    const uint32_t now = millis();

    if ((uint32_t)(now - g_lastButtonsMs) >= g_cfg.buttonPeriodMs)
    {
      g_lastButtonsMs = now;

      bool setPointChanged = false;
      if (g_upButton.pollPressEvent())
      {
        g_state.setPointPct += g_cfg.setPointStepPct;
        setPointChanged = true;
      }
      if (g_downButton.pollPressEvent())
      {
        g_state.setPointPct -= g_cfg.setPointStepPct;
        setPointChanged = true;
      }

      if (setPointChanged)
      {
        g_state.setPointPct = clampInt(g_state.setPointPct, g_cfg.setPointMinPct, g_cfg.setPointMaxPct);
        g_integralTerm = 0.0f;
        g_prevError = 0.0f;
        g_hasPrevError = false;
        g_state.controlOutputPct = 0.0f;
        g_state.controlDutyPct = 0.0f;
      }
    }

    if ((uint32_t)(now - g_lastSampleMs) >= g_cfg.samplePeriodMs)
    {
      g_lastSampleMs = now;

      float humidity = 0.0f;
      if (g_dht.readHumidityPercent(humidity))
      {
        g_state.humidityPct = humidity;
        g_state.humidityValid = true;
      }
      else
      {
        g_state.humidityValid = false;
      }
    }

    if ((uint32_t)(now - g_lastControlMs) >= g_cfg.controlPeriodMs)
    {
      const uint32_t dtMs = (uint32_t)(now - g_lastControlMs);
      g_lastControlMs = now;

      if (!g_state.humidityValid)
      {
        g_state.controlOutputPct = 0.0f;
        g_state.controlDutyPct = 0.0f;
        g_integralTerm = 0.0f;
        g_prevError = 0.0f;
        g_hasPrevError = false;
      }
      else
      {
        const float dtSec = (dtMs > 0) ? (dtMs / 1000.0f) : 0.001f;
        const float error = g_state.humidityPct - (float)g_state.setPointPct;

        const float outputMin = (g_cfg.outputMinPct < 0.0f) ? 0.0f : g_cfg.outputMinPct;

        g_integralTerm += g_cfg.ki * error * dtSec;
        g_integralTerm = clampFloat(g_integralTerm, outputMin, g_cfg.outputMaxPct);

        float derivative = 0.0f;
        if (g_hasPrevError)
        {
          derivative = (error - g_prevError) / dtSec;
        }

        float output = (g_cfg.kp * error) + g_integralTerm + (g_cfg.kd * derivative);
        output = clampFloat(output, outputMin, g_cfg.outputMaxPct);

        g_prevError = error;
        g_hasPrevError = true;

        g_state.controlOutputPct = output;
        g_state.controlDutyPct = clampFloat(output, 0.0f, 100.0f);
      }
    }

    g_state.actuatorRequestOn = g_state.humidityValid && (g_state.controlDutyPct > 0.0f);
  }

  State getState()
  {
    return g_state;
  }
}
