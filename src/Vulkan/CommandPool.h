#pragma once

#include "Device.h"
#include "VulkanApp.h"

namespace Vulkan {

class CommandPool {
  public:
    void init(const Device &device);
    void deinit(const Device &device);

    const VkCommandPool &getVkCommandPool() const { return m_CommandPool; } 

  private:
    VkCommandPool m_CommandPool;
};

} // namespace Vulkan