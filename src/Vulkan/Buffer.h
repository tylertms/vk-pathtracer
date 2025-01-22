#pragma once

#include "Device.h"
#include "VulkanApp.h"

namespace Vulkan {

uint32_t findMemoryType(const VkPhysicalDevice &physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
void createBuffer(const Device &device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);

} // namespace Vulkan