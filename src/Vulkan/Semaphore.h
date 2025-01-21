#pragma once

#include "VulkanApp.h"

namespace Vulkan {

class Semaphore {
  public:
    void init(const VkDevice &device);
    void deinit(const VkDevice &device);

  private:
    VkSemaphore m_Semaphore;
};

} // namespace Vulkan