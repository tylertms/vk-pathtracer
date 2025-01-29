#include "DescriptorSet.h"
#include "vulkan/vulkan_core.h"

#include <stdexcept>

namespace Vulkan {

void DescriptorSet::deinit(const VkDevice &device) {
    vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);
}

void DescriptorSet::createSet(const VkDevice &device, const SceneManager &sceneManager, const ImageView &accumulationImageView, const VkDescriptorPool &descriptorPool){
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &m_DescriptorSetLayout;

    if (vkAllocateDescriptorSets(device, &allocInfo, &m_DescriptorSet) != VK_SUCCESS) {
        throw std::runtime_error("ERROR: Failed to allocate descriptor sets.");
    }

    VkDescriptorBufferInfo uniformBufferInfo{};
    uniformBufferInfo.buffer = sceneManager.sceneUniform.getVkBuffer();
    uniformBufferInfo.offset = 0;
    uniformBufferInfo.range = sizeof(VKPT::SceneData);

    VkDescriptorImageInfo accumulatorImageInfo{};
    accumulatorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    accumulatorImageInfo.imageView = accumulationImageView.getVkImageView();
    accumulatorImageInfo.sampler = nullptr;

    VkDescriptorBufferInfo storageBufferInfo{};
    storageBufferInfo.buffer = sceneManager.getVkBuffer();
    storageBufferInfo.offset = 0;
    storageBufferInfo.range = sizeof(VKPT::StorageBuffer);

    VkWriteDescriptorSet descriptorWrites[5]{};
    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = m_DescriptorSet;
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo = &uniformBufferInfo;

    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = m_DescriptorSet;
    descriptorWrites[1].dstBinding = 1;
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pImageInfo = &accumulatorImageInfo;

    descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[2].dstSet = m_DescriptorSet;
    descriptorWrites[2].dstBinding = 2;
    descriptorWrites[2].dstArrayElement = 0;
    descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorWrites[2].descriptorCount = 1;
    descriptorWrites[2].pBufferInfo = &storageBufferInfo;

    vkUpdateDescriptorSets(device, 3, descriptorWrites, 0, nullptr);
}

void DescriptorSet::createLayout(const VkDevice &device) {
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding accumulationBinding{};
    accumulationBinding.binding = 1;
    accumulationBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    accumulationBinding.descriptorCount = 1;
    accumulationBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    accumulationBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding storageBufferBinding{};
    storageBufferBinding.binding = 2;
    storageBufferBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    storageBufferBinding.descriptorCount = 1;
    storageBufferBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    storageBufferBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding bindings[] = {
        uboLayoutBinding,
        accumulationBinding,
        storageBufferBinding
    };

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 3;
    layoutInfo.pBindings = bindings;

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("ERROR: Failed to create descriptor set layout.");
    }
}

} // namespace Vulkan
