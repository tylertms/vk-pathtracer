#pragma once

#include "VulkanApp.h"

namespace Vulkan {

class Window {
  public:
    GLFWwindow *init();
    void deinit();

    inline GLFWwindow *getGlfwWindow() const { return m_Window; }
    inline const uint32_t getWidth() const { return m_Width; }
    inline const uint32_t getHeight() const { return m_Height; }

  private:
    GLFWwindow *m_Window;

    uint32_t m_Width = 800;
    uint32_t m_Height = 600;
};

} // namespace Vulkan