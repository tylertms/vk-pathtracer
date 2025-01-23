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

  private:
    SceneObject *m_Instance;

    VkBuffer m_Buffer;
    VkDeviceMemory m_BufferMemory;
    void *m_BufferMapped;
};

} // namespace Vulkan
