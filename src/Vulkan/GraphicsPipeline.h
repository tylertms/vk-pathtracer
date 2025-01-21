#pragma once

#include "RenderPass.h"
#include "VulkanApp.h"

#include <vector>

namespace Vulkan {

class GraphicsPipeline {
  public:
    void init(const VkDevice &device, const VkFormat &swapChainFormat, const VkExtent2D &swapChainExtent);
    void deinit(const VkDevice &device);

  private:
    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR};

  private:
    VkPipeline m_GraphicsPipeline;
    VkPipelineLayout m_PipelineLayout;

    RenderPass m_RenderPass;

    VkPipelineDynamicStateCreateInfo getDynamicState();
    VkPipelineVertexInputStateCreateInfo getVertexInput();
    VkPipelineInputAssemblyStateCreateInfo getInputAssembly();
    VkViewport getViewport();
    VkRect2D getScissor();
    VkPipelineViewportStateCreateInfo getViewportState();
    VkPipelineRasterizationStateCreateInfo getRasterizer();
    VkPipelineMultisampleStateCreateInfo getMultisampling();
    VkPipelineColorBlendAttachmentState getColorBlendAttachment();
    VkPipelineColorBlendStateCreateInfo getColorBlending(VkPipelineColorBlendAttachmentState &colorBlendAttachment);
    VkPipelineLayoutCreateInfo getPipelineLayoutInfo();

  private:
    VkExtent2D ext_SwapChainExtent;
};

} // namespace Vulkan