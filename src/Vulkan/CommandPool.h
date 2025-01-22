#pragma once

#include "Device.h"
#include "VulkanApp.h"

namespace Vulkan {

class CommandPool {
  public:
    NO_COPY(CommandPool)
    CommandPool() = default;

    void init(const Device &device);
    void deinit(const Device &device);

    inline const VkCommandPool &getVkCommandPool() const { return m_CommandPool; } 

  private:
    VkCommandPool m_CommandPool;
};

} // namespace Vulkan