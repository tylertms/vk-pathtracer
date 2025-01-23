#include "Uniform.h"

#include "Buffer.h"

namespace Vulkan {

void Uniform::init(const Device &device) {
    VkDeviceSize bufferSize = sizeof(UniformObject);

    createBuffer(device, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_Buffer, m_BufferMemory);
    vkMapMemory(device.getVkDevice(), m_BufferMemory, 0, bufferSize, 0, &m_BufferMapped);
}

void Uniform::deinit(const VkDevice &device) {
    vkDestroyBuffer(device, m_Buffer, nullptr);
    vkFreeMemory(device, m_BufferMemory, nullptr);
}

} // namespace Vulkan
