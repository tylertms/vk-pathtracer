#pragma once

#include "Device.h"
#include "Instance.h"
#include "Window.h"

namespace VKAPP {

class Application {
  public:
    Application();
    ~Application();
    
    void run();

  private:
    Window m_AppWindow;
    Instance m_Instance;
    Device m_Device;
};

} // namespace VKAPP