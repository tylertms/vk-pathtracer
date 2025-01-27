#pragma once

#include "Uniform.h"
#include "VulkanApp.h"

namespace Vulkan {

class DescriptorSet {
  public:
    NO_COPY(DescriptorSet);
    DescriptorSet() = default;

    void createLayout(const VkDevice &device);
    void createSet(const VkDevice &device, const Uniform &uniform, const ImageView &accumulator, const ImageView &output, const VkDescriptorPool &descriptorPool);

    void deinit(const VkDevice &device);

    inline const VkDescriptorSetLayout &getVkDescriptorSetLayout() const { return m_DescriptorSetLayout; }
    inline const VkDescriptorSet &getVkDescriptorSet() const { return m_DescriptorSet; }

  private:
    VkDescriptorSet m_DescriptorSet;
    VkDescriptorSetLayout m_DescriptorSetLayout;
};

} // namespace Vulkan
