#pragma once

#include "Device.h"
#include "VulkanApp.h"

#include "../Core/Types/Scene.h"

namespace Vulkan {

class SceneUniform {
  public:
    NO_COPY(SceneUniform);
    SceneUniform() = default;

    void init(const Device &device);
    void deinit(const VkDevice &device);

    inline const VkBuffer &getVkBuffer() const { return m_Buffer; }

    inline const void incrementFramesRendered() { data.framesRendered++; }
    inline const void setFramesRendered(uint32_t count) { data.framesRendered = count; }
    inline const uint32_t getFramesRendered() const { return data.framesRendered; }

    void setCam(const VKPT::Camera &cam);
    void setCamAspectRatio(float aspectRatio);

    void submitUpdates() {
        memcpy(m_BufferMapped, &data, sizeof(VKPT::SceneData));
    }

    VKPT::SceneData data;

  private:
    VkBuffer m_Buffer;
    VkDeviceMemory m_BufferMemory;
    void *m_BufferMapped;
};

} // namespace Vulkan
