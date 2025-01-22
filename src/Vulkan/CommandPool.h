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

    VkCommandBuffer beginSingleTimeCommands(const VkDevice &device) const; 
    void endSingleTimeCommands(VkCommandBuffer &commandBuffer, const Device &device) const;
    void copyBuffer(const Device &device, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

  private:
    VkCommandPool m_CommandPool;
};

} // namespace Vulkan