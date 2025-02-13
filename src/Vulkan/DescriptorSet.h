#pragma once

#include "SceneManager.h"
#include "VulkanApp.h"

namespace Vulkan {

class DescriptorSet {
  public:
    NO_COPY(DescriptorSet);
    DescriptorSet() = default;

    void createLayout(const VkDevice &device);
    void createSet(const VkDevice &device, const SceneManager &sceneManager, const VkDescriptorPool &descriptorPool);
    void updateSet(const VkDevice &device, const SceneManager &sceneManager) const;

    void deinit(const VkDevice &device);

    inline const VkDescriptorSetLayout &getVkDescriptorSetLayout() const { return m_DescriptorSetLayout; }
    inline const VkDescriptorSet &getVkDescriptorSet() const { return m_DescriptorSet; }

  private:
    VkDescriptorSet m_DescriptorSet;
    VkDescriptorSetLayout m_DescriptorSetLayout;
};

} // namespace Vulkan
