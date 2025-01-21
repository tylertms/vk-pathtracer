#pragma once

#include "VkApp.h"

namespace VKAPP {

class Surface {
  public:
    void init(const VkInstance &instance, GLFWwindow *window);
    void deinit(const VkInstance &instance);

    VkSurfaceKHR getVkSurface() { return m_Surface; }
  private:
    VkSurfaceKHR m_Surface;
};

} // namespace VKAPP