#pragma once

#include "VulkanApp.h"

namespace Vulkan {

class Window {
  public:
    void init();
    void deinit();

    GLFWwindow *getGlfwWindow() const { return m_Window; }
    const uint32_t getWidth() const { return m_Width; }
    const uint32_t getHeight() const { return m_Height; }

  private:
    GLFWwindow *m_Window;

    uint32_t m_Width = 800;
    uint32_t m_Height = 600;
};

} // namespace Vulkan