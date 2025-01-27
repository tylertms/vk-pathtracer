#include "DescriptorPool.h"

#include <stdexcept>

namespace Vulkan {

void DescriptorPool::init(const VkDevice &device, uint32_t descriptorCount) {
    VkDescriptorPoolSize poolSizes[3]{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = descriptorCount;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    poolSizes[1].descriptorCount = descriptorCount;
    poolSizes[2].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    poolSizes[2].descriptorCount = descriptorCount;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 3;
    poolInfo.pPoolSizes = poolSizes;
    poolInfo.maxSets = descriptorCount;

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("ERROR: Failed to create descriptor pool.");
    }
}

void DescriptorPool::initImGui(const VkDevice &device, uint32_t descriptorCount) {
    descriptorCount *= 2;

    VkDescriptorPoolSize poolSizes[] = {
        {VK_DESCRIPTOR_TYPE_SAMPLER, descriptorCount},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, descriptorCount},
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, descriptorCount},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, descriptorCount},
        {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, descriptorCount},
        {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, descriptorCount},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, descriptorCount},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, descriptorCount},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, descriptorCount},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, descriptorCount},
        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, descriptorCount}};

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolInfo.maxSets = 0;
    for (auto poolSize : poolSizes) {
        poolInfo.maxSets += poolSize.descriptorCount;
    }
    poolInfo.poolSizeCount = 11;
    poolInfo.pPoolSizes = poolSizes;

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("ERROR: Failed to create imgui descriptor pool.");
    }
}

void DescriptorPool::deinit(const VkDevice &device) {
    vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);
}

} // namespace Vulkan
