#pragma once

#include "ImageView.h"
#include "VulkanApp.h"

namespace Vulkan {

class Framebuffer {
  public:
    void init(const VkDevice &device, const VkRenderPass &renderPass, const VkImageView &imageView, const VkExtent2D &extent);
    void deinit(const VkDevice &device);
    
  private:
    VkFramebuffer m_Framebuffer;
};

} // namespace Vulkan