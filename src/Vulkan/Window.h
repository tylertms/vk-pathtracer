#pragma once

#include "VkApp.h"

namespace VKAPP {

class Window {
  public:
    void init();
    ~Window();

    GLFWwindow *getWindow();

    uint32_t getWidth();
    uint32_t getHeight();

  private:
    GLFWwindow *m_Window;

    uint32_t m_Width = 800;
    uint32_t m_Height = 600;
};

} // namespace VKAPP