#include "DescriptorPool.h"

#include <stdexcept>

namespace Vulkan {

void DescriptorPool::init(const VkDevice &device, uint32_t descriptorCount) {
    VkDescriptorPoolSize poolSizes[2]{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = descriptorCount;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    poolSizes[1].descriptorCount = descriptorCount;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 2;
    poolInfo.pPoolSizes = poolSizes;
    poolInfo.maxSets = descriptorCount;

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("ERROR: Failed to create descriptor pool.");
    }
}

void DescriptorPool::deinit(const VkDevice &device) {
    vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);
}

} // namespace Vulkan