#pragma once

#include "CommandPool.h"
#include "GraphicsPipeline.h"
#include "RenderPass.h"
#include "VulkanApp.h"

namespace Vulkan {

class CommandBuffer {
  public:
    void init(const Device &device, const RenderPass &renderPass, const CommandPool &commandPool);

  private:
    VkCommandBuffer m_CommandBuffer;

    void record(const GraphicsPipeline &graphicsPipeline, const VkFramebuffer &framebuffer);

  private:
    RenderPass ext_RenderPass;
};

} // namespace Vulkan