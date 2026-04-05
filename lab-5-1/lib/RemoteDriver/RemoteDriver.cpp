#include "RemoteDriver.h"
#include <IRremote.hpp>

RemoteDriver::RemoteDriver(const Config &cfg) : m_cfg(cfg)
{
}

void RemoteDriver::setup()
{
  IrReceiver.begin(m_cfg.irReceivePin);
}

bool RemoteDriver::poll(Button &outButton, uint32_t &outRawCode)
{
  if (!IrReceiver.decode())
  {
    return false;
  }

  const uint32_t code = IrReceiver.decodedIRData.decodedRawData;
  IrReceiver.resume();

  outRawCode = code;

  outButton = Button::Unknown;
  if (code == m_cfg.codeUp)
  {
    outButton = Button::Up;
  }
  else if (code == m_cfg.codeRight)
  {
    outButton = Button::Right;
  }
  else if (code == m_cfg.codeDown)
  {
    outButton = Button::Down;
  }
  else if (code == m_cfg.codeLeft)
  {
    outButton = Button::Left;
  }

  return true;
}
