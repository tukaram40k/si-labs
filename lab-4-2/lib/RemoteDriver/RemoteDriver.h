#ifndef REMOTE_DRIVER_H
#define REMOTE_DRIVER_H

#include <Arduino.h>
#include <stdint.h>

// Decodes IR remote commands into ON/OFF requests.
class RemoteDriver {
public:
  struct Config {
    uint8_t irReceivePin;
    uint32_t codeOn;
    uint32_t codeOff;
  };

  explicit RemoteDriver(const Config& cfg);

  void setup();

  // Poll for one IR frame.
  // Returns true if *anything* was received.
  // - If the code matches codeOn/codeOff: outKnown=true and outOn is set.
  // - Otherwise: outKnown=false (unknown code)
  bool poll(bool& outOn, bool& outKnown, uint32_t& outRawCode);

private:
  Config m_cfg;
};

#endif
