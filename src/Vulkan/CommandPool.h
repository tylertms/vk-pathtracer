#pragma once

#include "Device.h"
#include "VulkanApp.h"

namespace Vulkan {

class CommandPool {
  public:
    void init(const Device &device);
    void deinit(const Device &device);
  private:
    VkCommandPool m_CommandPool;
};

} // namespace Vulkan