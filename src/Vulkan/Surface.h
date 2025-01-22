#pragma once

#include "VulkanApp.h"

namespace Vulkan {

class Surface {
  public:
    NO_COPY(Surface);
    Surface() = default;

    VkSurfaceKHR init(const VkInstance &instance, GLFWwindow *window);
    void deinit();

    const VkSurfaceKHR &getVkSurface() { return m_Surface; }

  private:
    VkSurfaceKHR m_Surface;

  private:
    VkInstance ext_Instance;
};

} // namespace Vulkan