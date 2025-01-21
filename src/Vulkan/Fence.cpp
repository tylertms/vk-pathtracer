#include "Fence.h"

#include <stdexcept>

namespace Vulkan {

void Fence::init(const VkDevice &device, bool signaled) {
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    if (signaled) {
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    }

    if (vkCreateFence(device, &fenceInfo, nullptr, &m_Fence) != VK_SUCCESS) {
        throw std::runtime_error("ERROR: Failed to create fence.");
    }
}

void Fence::deinit(const VkDevice &device) {
    vkDestroyFence(device, m_Fence, nullptr);
}

} // namespace Vulkan