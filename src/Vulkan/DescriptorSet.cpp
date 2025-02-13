#include "DescriptorSet.h"
#include "vulkan/vulkan_core.h"

#include <stdexcept>

namespace Vulkan {

void DescriptorSet::deinit(const VkDevice &device) {
    vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);
}

void DescriptorSet::createSet(const VkDevice &device, const SceneManager &sceneManager, const VkDescriptorPool &descriptorPool) {
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &m_DescriptorSetLayout;

    if (vkAllocateDescriptorSets(device, &allocInfo, &m_DescriptorSet) != VK_SUCCESS) {
        throw std::runtime_error("ERROR: Failed to allocate descriptor sets.");
    }

    updateSet(device, sceneManager);
}

void DescriptorSet::updateSet(const VkDevice &device, const SceneManager &sceneManager) const {
    VkDescriptorBufferInfo sceneDataInfo{};
    sceneDataInfo.buffer = sceneManager.getUniformBuffer();
    sceneDataInfo.offset = 0;
    sceneDataInfo.range = sizeof(VKPT::SceneData);

    VkDescriptorBufferInfo sceneStorageInfo{};
    sceneStorageInfo.buffer = sceneManager.getSceneStorage();
    sceneStorageInfo.offset = 0;
    sceneStorageInfo.range = sizeof(VKPT::SceneStorage);

    VkDescriptorImageInfo accumulatorImageInfo{};
    accumulatorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    accumulatorImageInfo.imageView = sceneManager.accumulationImageView.getVkImageView();
    accumulatorImageInfo.sampler = nullptr;

    VkDescriptorImageInfo textureInfo[MAX_TEXTURES]{};
    for (uint32_t i = 0; i < MAX_TEXTURES; i++) {
        textureInfo[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        textureInfo[i].imageView = sceneManager.textures[i].getVkImageView();
        textureInfo[i].sampler = nullptr;
    }

    VkDescriptorImageInfo samplerInfo{};
    samplerInfo.sampler = sceneManager.textures[0].getVkSampler();

    VkWriteDescriptorSet descriptorWrites[5]{};
    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = m_DescriptorSet;
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo = &sceneDataInfo;

    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = m_DescriptorSet;
    descriptorWrites[1].dstBinding = 1;
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pBufferInfo = &sceneStorageInfo;

    descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[2].dstSet = m_DescriptorSet;
    descriptorWrites[2].dstBinding = 2;
    descriptorWrites[2].dstArrayElement = 0;
    descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    descriptorWrites[2].descriptorCount = 1;
    descriptorWrites[2].pImageInfo = &accumulatorImageInfo;

    descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[3].dstSet = m_DescriptorSet;
    descriptorWrites[3].dstBinding = 3;
    descriptorWrites[3].dstArrayElement = 0;
    descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    descriptorWrites[3].descriptorCount = MAX_TEXTURES;
    descriptorWrites[3].pImageInfo = textureInfo;

    descriptorWrites[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[4].dstSet = m_DescriptorSet;
    descriptorWrites[4].dstBinding = 4;
    descriptorWrites[4].dstArrayElement = 0;
    descriptorWrites[4].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
    descriptorWrites[4].descriptorCount = 1;
    descriptorWrites[4].pImageInfo = &samplerInfo;

    vkUpdateDescriptorSets(device, 5, descriptorWrites, 0, nullptr);
}

void DescriptorSet::createLayout(const VkDevice &device) {
    VkDescriptorSetLayoutBinding sceneDataBinding{};
    sceneDataBinding.binding = 0;
    sceneDataBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    sceneDataBinding.descriptorCount = 1;
    sceneDataBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    sceneDataBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding sceneStorageBinding{};
    sceneStorageBinding.binding = 1;
    sceneStorageBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    sceneStorageBinding.descriptorCount = 1;
    sceneStorageBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    sceneStorageBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding accumulationBinding{};
    accumulationBinding.binding = 2;
    accumulationBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    accumulationBinding.descriptorCount = 1;
    accumulationBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    accumulationBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding textureBinding{};
    textureBinding.binding = 3;
    textureBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    textureBinding.descriptorCount = MAX_TEXTURES;
    textureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    textureBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding samplerBinding{};
    samplerBinding.binding = 4;
    samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
    samplerBinding.descriptorCount = 1;
    samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    samplerBinding.pImmutableSamplers = nullptr;


    VkDescriptorSetLayoutBinding bindings[] = {
        sceneDataBinding,
        sceneStorageBinding,
        accumulationBinding,
        textureBinding,
        samplerBinding
    };

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 5;
    layoutInfo.pBindings = bindings;

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("ERROR: Failed to create descriptor set layout.");
    }
}

} // namespace Vulkan
