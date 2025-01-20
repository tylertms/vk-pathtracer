#pragma once
#include "Window.h"

namespace Vulkan {

class Application {
   public:
    ~Application();
    void run();

   private:
    Window m_AppWindow;
};

}  // namespace Vulkan