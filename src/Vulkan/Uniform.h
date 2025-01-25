#pragma once

#include "Device.h"
#include "Scene.h"
#include "VulkanApp.h"

namespace Vulkan {

class Uniform {
  public:
    NO_COPY(Uniform);
    Uniform() = default;

    void init(const Device &device, SceneObject *uniform);
    void deinit(const VkDevice &device);

    inline const VkBuffer &getVkBuffer() const { return m_Buffer; }

    void submitUpdates() {
        memcpy(m_BufferMapped, m_Instance, sizeof(SceneObject));
    }

void submitFramesRendered() {
    size_t offset = offsetof(SceneObject, framesRendered);
    char* bufferPtr = static_cast<char*>(m_BufferMapped) + offset;
    char* instancePtr = reinterpret_cast<char*>(m_Instance) + offset;
    std::memcpy(bufferPtr, instancePtr, sizeof(uint32_t));
}

  private:
    SceneObject *m_Instance;

    VkBuffer m_Buffer;
    VkDeviceMemory m_BufferMemory;
    void *m_BufferMapped;
};

} // namespace Vulkan
