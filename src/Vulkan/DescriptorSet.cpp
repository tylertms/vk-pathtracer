#include "DescriptorSet.h"
#include "vulkan/vulkan_core.h"

#include <stdexcept>

namespace Vulkan {

void DescriptorSet::deinit(const VkDevice &device) {
    vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);
}

void DescriptorSet::createSet(const VkDevice &device, const SceneManager &sceneManager, const ImageView &accumulationImageView, const VkDescriptorPool &descriptorPool) {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &m_DescriptorSetLayout;

    if (vkAllocateDescriptorSets(device, &allocInfo, &m_DescriptorSet) != VK_SUCCESS) {
        throw std::runtime_error("ERROR: Failed to allocate descriptor sets.");
    }

    VkDescriptorBufferInfo uniformBufferInfo{};
    uniformBufferInfo.buffer = sceneManager.getUniformBuffer();
    uniformBufferInfo.offset = 0;
    uniformBufferInfo.range = sizeof(VKPT::SceneData);

    VkDescriptorImageInfo envSamplerInfo{};
    envSamplerInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    envSamplerInfo.imageView = sceneManager.getEnvImageView();
    envSamplerInfo.sampler = sceneManager.getEnvSampler();

    VkDescriptorImageInfo accumulatorImageInfo{};
    accumulatorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    accumulatorImageInfo.imageView = accumulationImageView.getVkImageView();
    accumulatorImageInfo.sampler = nullptr;

    VkDescriptorBufferInfo SceneStorageInfo{};
    SceneStorageInfo.buffer = sceneManager.getSceneStorage();
    SceneStorageInfo.offset = 0;
    SceneStorageInfo.range = sizeof(VKPT::SceneStorage);

    VkWriteDescriptorSet descriptorWrites[4]{};
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
    descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[2].descriptorCount = 1;
    descriptorWrites[2].pImageInfo = &envSamplerInfo;

    descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[3].dstSet = m_DescriptorSet;
    descriptorWrites[3].dstBinding = 3;
    descriptorWrites[3].dstArrayElement = 0;
    descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorWrites[3].descriptorCount = 1;
    descriptorWrites[3].pBufferInfo = &SceneStorageInfo;

    vkUpdateDescriptorSets(device, 4, descriptorWrites, 0, nullptr);
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

    VkDescriptorSetLayoutBinding envSamplerBinding{};
    envSamplerBinding.binding = 2;
    envSamplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    envSamplerBinding.descriptorCount = 1;
    envSamplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    envSamplerBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding SceneStorageBinding{};
    SceneStorageBinding.binding = 3;
    SceneStorageBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    SceneStorageBinding.descriptorCount = 1;
    SceneStorageBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    SceneStorageBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding bindings[] = {
        uboLayoutBinding,
        accumulationBinding,
        envSamplerBinding,
        SceneStorageBinding};

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 4;
    layoutInfo.pBindings = bindings;

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("ERROR: Failed to create descriptor set layout.");
    }
}

} // namespace Vulkan
