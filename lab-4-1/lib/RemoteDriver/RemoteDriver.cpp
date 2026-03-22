#include "RemoteDriver.h"
#include <IRremote.hpp>

RemoteDriver::RemoteDriver(const Config& cfg) : m_cfg(cfg) {
}

void RemoteDriver::setup() {
  IrReceiver.begin(m_cfg.irReceivePin);
}

bool RemoteDriver::poll(bool& outOn, bool& outKnown, uint32_t& outRawCode) {
  if (!IrReceiver.decode()) {
    return false;
  }

  const uint32_t code = IrReceiver.decodedIRData.decodedRawData;
  IrReceiver.resume();

  outRawCode = code;
  outKnown = false;

  if (code == m_cfg.codeOn) {
    outOn = true;
    outKnown = true;
    return true;
  }
  if (code == m_cfg.codeOff) {
    outOn = false;
    outKnown = true;
    return true;
  }

  return true;
}
