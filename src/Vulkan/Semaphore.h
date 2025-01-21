#pragma once

#include "VulkanApp.h"

namespace Vulkan {

class Semaphore {
  public:
    void init(const VkDevice &device);
    void deinit(const VkDevice &device);

    inline const VkSemaphore &getVkSemaphore() const { return m_Semaphore; }

  private:
    VkSemaphore m_Semaphore;
};

} // namespace Vulkan