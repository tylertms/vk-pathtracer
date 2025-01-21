#pragma once

#include "VulkanApp.h"

namespace Vulkan {

class RenderPass {
  public:
    void init(const VkDevice &device, const VkFormat &format);
    void deinit(const VkDevice &device);

    const VkRenderPass &getVkRenderPass() const { return m_RenderPass; }

  private:
    VkRenderPass m_RenderPass;
};

} // namespace Vulkan