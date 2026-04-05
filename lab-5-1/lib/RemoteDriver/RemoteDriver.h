#ifndef REMOTE_DRIVER_H
#define REMOTE_DRIVER_H

#include <Arduino.h>
#include <stdint.h>

// Decodes IR remote commands into buttons.
class RemoteDriver {
public:
  struct Config {
    uint8_t irReceivePin;

    // Button codes (see src/IRCodes.cpp)
    uint32_t codeUp;
    uint32_t codeRight;
    uint32_t codeDown;
    uint32_t codeLeft;
  };

  enum class Button : uint8_t {
    Unknown = 0,
    Up,
    Right,
    Down,
    Left,
  };

  explicit RemoteDriver(const Config& cfg);

  void setup();

  // Poll for one IR frame.
  // Returns true if *anything* was received.
  // outButton is set to Unknown if the code wasn't recognized.
  bool poll(Button& outButton, uint32_t& outRawCode);

private:
  Config m_cfg;
};

#endif
