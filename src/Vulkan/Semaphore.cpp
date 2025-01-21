#include "Semaphore.h"

#include <stdexcept>

namespace Vulkan {

void Semaphore::init(const VkDevice &device) {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_Semaphore) != VK_SUCCESS) {
        throw std::runtime_error("ERROR: Failed to create semaphore.");
    }
}

void Semaphore::deinit(const VkDevice &device) {
    vkDestroySemaphore(device, m_Semaphore, nullptr);
}

} // namespace Vulkan