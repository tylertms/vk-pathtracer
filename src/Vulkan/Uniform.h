#pragma once

#include "Device.h"
#include "VulkanApp.h"

#include "../Core/Types/SceneObject.h"

namespace Vulkan {

class Uniform {
  public:
    NO_COPY(Uniform);
    Uniform() = default;

    void init(const Device &device, VKPT::SceneObject *uniform);
    void deinit(const VkDevice &device);

    inline const VkBuffer &getVkBuffer() const { return m_Buffer; }

    void submitUpdates() {
        memcpy(m_BufferMapped, m_Instance, sizeof(VKPT::SceneObject));
    }

void submitFramesRendered() {
    size_t offset = offsetof(VKPT::SceneObject, framesRendered);
    char* bufferPtr = static_cast<char*>(m_BufferMapped) + offset;
    char* instancePtr = reinterpret_cast<char*>(m_Instance) + offset;
    std::memcpy(bufferPtr, instancePtr, sizeof(uint32_t));
}

  private:
    VKPT::SceneObject *m_Instance;

    VkBuffer m_Buffer;
    VkDeviceMemory m_BufferMemory;
    void *m_BufferMapped;
};

} // namespace Vulkan
