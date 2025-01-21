#pragma once

#include "Device.h"
#include "Instance.h"
#include "Surface.h"
#include "Window.h"

namespace VKAPP {

class Application {
  public:
    Application();
    ~Application();

    void run();

    const Window &getWindow() { return m_Window; }
    const Instance &getInstance() { return m_Instance; }
    const Device &getDevice() { return m_Device; }
    const Surface &getSurface() { return m_Surface; }

  private:
    Window m_Window;
    Instance m_Instance;
    Device m_Device;
    Surface m_Surface;
};

extern Application g_AppInstance;

} // namespace VKAPP