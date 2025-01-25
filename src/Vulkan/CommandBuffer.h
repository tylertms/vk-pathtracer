#pragma once

#include "CommandPool.h"
#include "GraphicsPipeline.h"
#include "VulkanApp.h"

#include "../Interface/UserInterface.h"

namespace Vulkan {

class CommandBuffer {
  public:
    NO_COPY(CommandBuffer);
    CommandBuffer() = default;

    void init(const Device &device, const CommandPool &commandPool);

    inline const VkCommandBuffer &getVkCommandBuffer() const { return m_CommandBuffer; }
    void record(const GraphicsPipeline &graphicsPipeline, Scene &scene, Interface::UserInterface &interface, const VkFramebuffer &framebuffer, const VkDescriptorSet &descriptorSet);

  private:
    VkCommandBuffer m_CommandBuffer;
};

} // namespace Vulkan
