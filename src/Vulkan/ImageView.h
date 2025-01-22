#pragma once

#include "Buffer.h"
#include "VulkanApp.h"

namespace Vulkan {

class CommandPool;

class ImageView {
  public:
    NO_COPY(ImageView);
    ImageView() = default;

    void init(const VkDevice &device, const VkImage &image, const VkFormat &format);
    void deinit(const VkDevice &device);

    void createImage(const Device &device, VkExtent2D extent, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
    void transitionImageLayout(const Device &device, const CommandPool &commandPool, VkImageLayout oldLayout, VkImageLayout newLayout);

    inline const VkImage &getVkImage() const { return m_Image; }
    inline const VkImageView &getVkImageView() const { return m_ImageView; }

  private:
    VkImageView m_ImageView = nullptr;
    VkImage m_Image = nullptr;
    VkDeviceMemory m_ImageMemory = nullptr;

    VkFormat m_Format;
};

} // namespace Vulkan