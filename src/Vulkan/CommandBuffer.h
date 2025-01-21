#pragma once

#include "CommandPool.h"
#include "GraphicsPipeline.h"
#include "RenderPass.h"
#include "VulkanApp.h"

namespace Vulkan {

class CommandBuffer {
  public:
    void init(const Device &device, const CommandPool &commandPool);

    inline const VkCommandBuffer &getVkCommandBuffer() const { return m_CommandBuffer; }
    void record(const GraphicsPipeline &graphicsPipeline, const VkFramebuffer &framebuffer);

  private:
    VkCommandBuffer m_CommandBuffer;
};

} // namespace Vulkan