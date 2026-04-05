#include "task_conditioning.h"

#include "ButtonDriver.h"
#include "Dht11Driver.h"

namespace
{
  TaskConditioning::Config g_cfg{3, 9, 8, true, 30, 55, 30, 90, 1, 3.0f, 30, 1200, 100};
  TaskConditioning::State g_state{};

  ButtonDriver g_upButton;
  ButtonDriver g_downButton;
  Dht11Driver g_dht;

  uint32_t g_lastButtonsMs = 0;
  uint32_t g_lastSampleMs = 0;
  uint32_t g_lastControlMs = 0;

  int clampInt(int v, int lo, int hi)
  {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
  }

  void updateThresholds()
  {
    g_state.lowerThresholdPct = (float)g_state.setPointPct - g_cfg.hysteresisPct;
    g_state.upperThresholdPct = (float)g_state.setPointPct + g_cfg.hysteresisPct;
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
    g_state.relayRequestOn = false;
    updateThresholds();

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
        updateThresholds();
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
      g_lastControlMs = now;

      if (!g_state.humidityValid)
      {
        return;
      }

      if (!g_state.relayRequestOn && g_state.humidityPct < g_state.lowerThresholdPct)
      {
        g_state.relayRequestOn = true;
      }
      else if (g_state.relayRequestOn && g_state.humidityPct > g_state.upperThresholdPct)
      {
        g_state.relayRequestOn = false;
      }
    }
  }

  State getState()
  {
    return g_state;
  }
}
