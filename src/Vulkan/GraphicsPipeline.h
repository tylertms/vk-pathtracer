#pragma once

#include "RenderPass.h"
#include "VulkanApp.h"

#include <vector>

namespace Vulkan {

class GraphicsPipeline {
  public:
    void init(const VkDevice &device, const VkFormat &swapChainFormat, const VkExtent2D &swapChainExtent);
    void deinit(const VkDevice &device);

    inline const VkPipeline &getVkPipeline() const { return m_GraphicsPipeline; }
    inline const RenderPass &getRenderPass() const { return m_RenderPass; }
    inline const VkRenderPass &getVkRenderPass() const { return m_RenderPass.getVkRenderPass(); }

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
    VkPipelineLayoutCreateInfo getPipelineLayoutInfo() const;

  private:
    VkExtent2D ext_SwapChainExtent;
};

} // namespace Vulkan