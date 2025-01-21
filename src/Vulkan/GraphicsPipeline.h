#pragma once

#include "VulkanApp.h"

namespace Vulkan {

class GraphicsPipeline {
  public:
    void init(const VkDevice &device, const VkExtent2D swapChainExtent);
    void deinit(const VkDevice &device);

  private:
    VkPipelineLayout m_PipelineLayout;

    VkPipelineDynamicStateCreateInfo getDynamicState();
    VkPipelineVertexInputStateCreateInfo getVertexInput();
    VkPipelineInputAssemblyStateCreateInfo getInputAssembly();
    VkViewport getViewport();
    VkRect2D getScissor();
    VkPipelineViewportStateCreateInfo getViewportState();
    VkPipelineRasterizationStateCreateInfo getRasterizer();
    VkPipelineMultisampleStateCreateInfo getMultisampling();
    VkPipelineColorBlendAttachmentState getColorBlendAttachment();
    VkPipelineColorBlendStateCreateInfo getColorBlendState();
    VkPipelineLayoutCreateInfo getPipelineLayoutInfo();

  private:
    VkExtent2D ext_SwapChainExtent;
};

} // namespace Vulkan