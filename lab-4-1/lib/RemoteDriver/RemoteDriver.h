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

  // Returns true if a command was received, with outOn set accordingly.
  bool poll(bool& outOn);

private:
  Config m_cfg;
};

#endif
