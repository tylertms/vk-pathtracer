#pragma once

#include "Instance.h"
#include "Window.h"

namespace VkApp {

class Application {
  public:
    ~Application();
    void run();

  private:
    Window m_AppWindow;
    Instance m_Instance;
};

} // namespace VkApp