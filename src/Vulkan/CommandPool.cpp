#include "CommandPool.h"
#include "vulkan/vulkan_core.h"

#include <stdexcept>

namespace Vulkan {

void CommandPool::init(const Device &device) {
    VkPhysicalDevice physicalDevice = device.getVkPhysicalDevice();
    Device::QueueFamilyIndices queueFamilyIndices = device.findQueueFamilies(physicalDevice, nullptr);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(device.getVkDevice(), &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS) {
        throw std::runtime_error("ERROR: Failed to create command pool.");
    }
}

void CommandPool::deinit(const Device &device) {
    vkDestroyCommandPool(device.getVkDevice(), m_CommandPool, nullptr);
}

VkCommandBuffer CommandPool::beginSingleTimeCommands(const VkDevice &device) const {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_CommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void CommandPool::endSingleTimeCommands(VkCommandBuffer &commandBuffer, const Device &device) const {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(device.getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(device.getGraphicsQueue());

    vkFreeCommandBuffers(device.getVkDevice(), m_CommandPool, 1, &commandBuffer);
}

void CommandPool::copyBuffer(const Device &device, const VkBuffer &srcBuffer, VkBuffer &dstBuffer, VkDeviceSize size) const {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(device.getVkDevice());

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer, device);
}

void CommandPool::copyBufferToImage(const Device &device, const VkBuffer &srcBuffer, VkImage &dstimage, VkExtent2D extent) const {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(device.getVkDevice());

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
        extent.width,
        extent.height,
        1
    };

    vkCmdCopyBufferToImage(
        commandBuffer,
        srcBuffer,
        dstimage,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );

    endSingleTimeCommands(commandBuffer, device);
}

} // namespace Vulkan
