#pragma once

#include "VulkanApp.h"

namespace Vulkan {

class Fence {
  public:
    NO_COPY(Fence);
    Fence() = default;
    
    void init(const VkDevice &device, bool signaled);
    void deinit(const VkDevice &device);

    inline const VkFence &getVkFence() const { return m_Fence; }

  private:
    VkFence m_Fence;
};

} // namespace Vulkan