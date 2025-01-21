#include "CommandPool.h"

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

} // namespace Vulkan
