#pragma once

#include "VulkanApp.h"

namespace Vulkan {

class Window {
  public:
    void init();
    void deinit();

    GLFWwindow *getVkWindow();

    uint32_t getWidth();
    uint32_t getHeight();

  private:
    GLFWwindow *m_Window;

    uint32_t m_Width = 800;
    uint32_t m_Height = 600;
};

} // namespace Vulkan