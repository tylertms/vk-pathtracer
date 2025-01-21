#include "Framebuffer.h"

#include <stdexcept>

namespace Vulkan {

void Framebuffer::init(const VkDevice &device, const VkRenderPass &renderPass, const VkImageView &imageView, const VkExtent2D &extent) {
    VkImageView attachments[] = {
        imageView};

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &m_Framebuffer) != VK_SUCCESS) {
        throw std::runtime_error("ERROR: Failed to create framebuffer.");
    }
}

void Framebuffer::deinit(const VkDevice &device) {
    vkDestroyFramebuffer(device, m_Framebuffer, nullptr);
}

} // namespace Vulkan