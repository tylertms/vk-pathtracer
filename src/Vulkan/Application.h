#pragma once

#include "Instance.h"
#include "Window.h"

namespace Vulkan {

class Application {
  public:
    ~Application();
    void run();

  private:
    Window m_AppWindow;
    Instance m_Instance;
};

} // namespace Vulkan