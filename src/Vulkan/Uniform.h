#pragma once

#include "Device.h"
#include "VulkanApp.h"

#include "glm/glm.hpp"

namespace Vulkan {

class Uniform {
  public:
    NO_COPY(Uniform);
    Uniform() = default;

    void init(const Device &device);
    void deinit(const VkDevice &device);

    inline const VkBuffer &getVkBuffer() const { return m_Buffer; }

    void updateFramesRendered(uint32_t framesRendered) {
        m_Instance.framesRendered = framesRendered;
    }

    void submitUpdates() {
        memcpy(m_BufferMapped, &m_Instance, sizeof(Object));
    }

  public:
    struct Camera {
        glm::vec3 lookFrom;
        float vfov;
        glm::vec3 lookAt;
        float aperture;
        float aspectRatio;
    };

    struct Object {
        uint32_t framesRendered;
        alignas(16) Camera cam;
    };

  private:
    Object m_Instance;

    VkBuffer m_Buffer;
    VkDeviceMemory m_BufferMemory;
    void *m_BufferMapped;
};

} // namespace Vulkan