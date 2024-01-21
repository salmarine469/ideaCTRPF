#include "Led.hpp"

namespace CTRPluginFramework
{
  Led::Led(MenuEntry *entry)
  {
    this->entry = entry;
  }

  void Led::setSmoothing(u8 smoothing)
  {
    this->data.smoothing = smoothing;
  }

  void Led::setColor(u8 r, u8 g, u8 b)
  {
    this->r = r;
    this->g = g;
    this->b = b;

    for (int i = 0; i < 32; i++)
    {
      this->data.r[i] = r;
      this->data.g[i] = g;
      this->data.b[i] = b;
    }

    if (hasColor() && !this->running)
      this->running = true;
  }

  bool Led::hasColor()
  {
    return this->r != 0 || this->g != 0 || this->b != 0;
  }

  void Led::update()
  {
    if (!hasColor() && !this->running)
      return;

    Handle serviceHandle;
    Result res = srvGetServiceHandle(&serviceHandle, "ptm:sysm");
    if (res != 0 && this->entry->IsActivated())
    {
      this->entry->Disable();
      OSD::Notify("Error: Failed to enable the LED mode.");
      return;
    }

    u32* prm = getThreadCommandBuffer();
    prm[0] = 0x8010640;
    memcpy(&prm[1], &data, 0x64);
    svcSendSyncRequest(serviceHandle);
    svcCloseHandle(serviceHandle);

    if (!hasColor())
      this->running = false;
  }
}