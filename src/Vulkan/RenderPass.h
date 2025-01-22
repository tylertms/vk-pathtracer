#pragma once

#include "VulkanApp.h"

namespace Vulkan {

class RenderPass {
  public:
    NO_COPY(RenderPass)
    RenderPass() = default;

    void init(const VkDevice &device, const VkFormat &format, const VkExtent2D &extent);
    void deinit(const VkDevice &device);

    VkRenderPassBeginInfo getBeginInfo(const VkFramebuffer &framebuffer) const;
    void updateExtent(const VkExtent2D &extent);

    inline const VkRenderPass &getVkRenderPass() const { return m_RenderPass; }

  private:
    VkRenderPass m_RenderPass;

  private:
    VkExtent2D ext_Extent;
};

} // namespace Vulkan