#pragma once

#include "VulkanApp.h"
#include <cstdint>

namespace Vulkan {

class Window {
  public:
    NO_COPY(Window)
    Window() = default;

    GLFWwindow *init();
    void deinit();

    inline GLFWwindow *getGlfwWindow() const { return m_Window; }
    inline const uint32_t getWidth() const { return m_Width; }
    inline const uint32_t getHeight() const { return m_Height; }
    inline const float getAspectRatio() const { return (float)m_Width / m_Height; }

    inline void setWindowSize(uint32_t width, uint32_t height) {
        m_Width = width;
        m_Height = height;
    }

    static void onResize(GLFWwindow *window, int width, int height);
    bool wasResized();
    void clearResized();

  private:
    GLFWwindow *m_Window;
    bool m_Resized = false;

    uint32_t m_Width = 1920;
    uint32_t m_Height = 1080;
};

} // namespace Vulkan
