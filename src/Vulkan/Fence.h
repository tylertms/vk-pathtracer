#pragma once

#include "VulkanApp.h"

namespace Vulkan {

class Fence {
  public:
    void init(const VkDevice &device);
    void deinit(const VkDevice &device);
  private:
    VkFence m_Fence;
};

} // namespace Vulkan