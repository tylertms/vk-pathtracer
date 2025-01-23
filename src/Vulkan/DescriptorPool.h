#pragma once

#include "VulkanApp.h"

namespace Vulkan {

class DescriptorPool {
  public:
    NO_COPY(DescriptorPool);
    DescriptorPool() = default;

    void init(const VkDevice &device, uint32_t descriptorCount);
    void initImGui(const VkDevice &device, uint32_t descriptorCount);
    void deinit(const VkDevice &device);

    inline const VkDescriptorPool &getVkDescriptorPool() const { return m_DescriptorPool; }

  private:
    VkDescriptorPool m_DescriptorPool;
};

} // namespace Vulkan
