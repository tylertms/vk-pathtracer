#pragma once

#include "CommandPool.h"
#include "GraphicsPipeline.h"
#include "RenderPass.h"
#include "VulkanApp.h"

namespace Vulkan {

class CommandBuffer {
  public:
    void init(const Device &device, const RenderPass &renderPass, const CommandPool &commandPool);

    inline const VkCommandBuffer &getVkCommandBuffer() const { return m_CommandBuffer; }
    void record(const GraphicsPipeline &graphicsPipeline, const VkFramebuffer &framebuffer);

  private:
    VkCommandBuffer m_CommandBuffer;

  private:
    RenderPass ext_RenderPass;
};

} // namespace Vulkan