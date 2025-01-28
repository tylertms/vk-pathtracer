#pragma once

#include "CommandPool.h"
#include "GraphicsPipeline.h"
#include "SceneManager.h"
#include "VulkanApp.h"

#include "../Core/Interface/UserInterface.h"

namespace Vulkan {

class CommandBuffer {
  public:
    NO_COPY(CommandBuffer);
    CommandBuffer() = default;

    void init(const Device &device, const CommandPool &commandPool);

    inline const VkCommandBuffer &getVkCommandBuffer() const { return m_CommandBuffer; }
    void record(const GraphicsPipeline &graphicsPipeline, SceneManager &sceneManager, Interface::UserInterface &interface, const VkFramebuffer &framebuffer, const VkDescriptorSet &descriptorSet, const Window &window, ImVec2 &position, ImVec2 &extent);

  private:
    VkCommandBuffer m_CommandBuffer;
};

} // namespace Vulkan
