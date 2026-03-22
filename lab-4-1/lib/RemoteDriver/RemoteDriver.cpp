#include "RemoteDriver.h"
#include <IRremote.hpp>

RemoteDriver::RemoteDriver(const Config& cfg) : m_cfg(cfg) {
}

void RemoteDriver::setup() {
  IrReceiver.begin(m_cfg.irReceivePin);
}

bool RemoteDriver::poll(bool& outOn) {
  if (!IrReceiver.decode()) {
    return false;
  }

  const uint32_t code = IrReceiver.decodedIRData.decodedRawData;
  IrReceiver.resume();

  if (code == m_cfg.codeOn) {
    outOn = true;
    return true;
  }
  if (code == m_cfg.codeOff) {
    outOn = false;
    return true;
  }

  return false;
}
