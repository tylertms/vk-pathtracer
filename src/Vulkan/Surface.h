#pragma once

#include "VulkanApp.h"

namespace Vulkan {

class Surface {
  public:
    void init(const VkInstance &instance, GLFWwindow *window);
    void deinit();

    VkSurfaceKHR getVkSurface() { return m_Surface; }

  private:
    VkSurfaceKHR m_Surface;

  private:
    VkInstance ext_Instance;
};

} // namespace Vulkan