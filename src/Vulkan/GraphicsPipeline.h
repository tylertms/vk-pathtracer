#pragma once

#include "RenderPass.h"
#include "VulkanApp.h"

#include <vector>

namespace Vulkan {

class GraphicsPipeline {
  public:
    NO_COPY(GraphicsPipeline);
    GraphicsPipeline() = default;

    void init(const VkDevice &device, const VkDescriptorSetLayout &descriptorSetLayout, const VkFormat &swapChainFormat, const VkExtent2D &swapChainExtent);
    void deinit(const VkDevice &device);

    inline const VkPipeline &getVkPipeline() const { return m_GraphicsPipeline; }
    inline const VkPipelineLayout &getVkPipelineLayout() const { return m_PipelineLayout; }
    inline const RenderPass &getRenderPass() const { return m_RenderPass; }
    inline const VkRenderPass &getVkRenderPass() const { return m_RenderPass.getVkRenderPass(); }

    void updateExtent(const VkExtent2D &extent);
    VkViewport getViewport() const;
    VkRect2D getScissor() const;

  private:
    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR};

  private:
    VkPipeline m_GraphicsPipeline;
    VkPipelineLayout m_PipelineLayout;

    RenderPass m_RenderPass;

    VkPipelineDynamicStateCreateInfo getDynamicState() const;
    VkPipelineVertexInputStateCreateInfo getVertexInput() const;
    VkPipelineInputAssemblyStateCreateInfo getInputAssembly() const;
    VkPipelineViewportStateCreateInfo getViewportState() const;
    VkPipelineRasterizationStateCreateInfo getRasterizer() const;
    VkPipelineMultisampleStateCreateInfo getMultisampling() const;
    VkPipelineColorBlendAttachmentState getColorBlendAttachment() const;
    VkPipelineColorBlendStateCreateInfo getColorBlending(VkPipelineColorBlendAttachmentState &colorBlendAttachment) const;
    VkPipelineLayoutCreateInfo getPipelineLayoutInfo(const VkDescriptorSetLayout &descriptorSetLayout) const;

  private:
    VkExtent2D ext_SwapChainExtent;
};

} // namespace Vulkan
