#pragma once

#include "CommandPool.h"
#include "GraphicsPipeline.h"
#include "Interface.h"
#include "RenderPass.h"
#include "VulkanApp.h"

namespace Vulkan {

class CommandBuffer {
  public:
    NO_COPY(CommandBuffer);
    CommandBuffer() = default;

    void init(const Device &device, const CommandPool &commandPool);

    inline const VkCommandBuffer &getVkCommandBuffer() const { return m_CommandBuffer; }
    void record(const GraphicsPipeline &graphicsPipeline, const Interface &interface, const VkFramebuffer &framebuffer, const VkDescriptorSet &descriptorSet);

  private:
    VkCommandBuffer m_CommandBuffer;
};

} // namespace Vulkan
