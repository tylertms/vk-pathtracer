#pragma once

#include "VulkanApp.h"

namespace Vulkan {

class ImageView {
  public:
    NO_COPY(ImageView);
    ImageView() = default;
    
    void init(const VkDevice &device, const VkImage &image, const VkFormat &format);
    void deinit(const VkDevice &device);

    inline const VkImageView &getVkImageView() const { return m_ImageView; }

  private:
    VkImageView m_ImageView;
};

} // namespace Vulkan