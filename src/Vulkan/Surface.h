#pragma once

#include "VulkanApp.h"

namespace Vulkan {

class Surface {
  public:
    void init(const VkInstance &instance, GLFWwindow *window);
    void deinit(const VkInstance &instance);

    VkSurfaceKHR getVkSurface() { return m_Surface; }
  private:
    VkSurfaceKHR m_Surface;
};

} // namespace Vulkan