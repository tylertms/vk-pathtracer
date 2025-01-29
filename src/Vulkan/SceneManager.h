#pragma once

#include <cstring>
#include <iostream>
#include <stdint.h>
#include <vector>

#include "Device.h"

#include "../Core/Types/Scene.h"
#include "../Core/Utils/Matrix.h"
#include "../Core/Loader/GLTFLoader.h"
#include "VulkanApp.h"
#include "vulkan/vulkan_core.h"

namespace Vulkan {

class SceneManager {
  public:
    NO_COPY(SceneManager)
    SceneManager() = default;

    void init(const Device &device);
    void deinit(const VkDevice &device);

    inline const VkBuffer &getUniformBuffer() const { return m_UniformBuffer; }
    inline const VkBuffer &getStorageBuffer() const { return m_StorageBuffer; }

    void resetAccumulation();
    bool resetOccurred();
    void submitUniformUpdates();
    void submitStorageUpdatesIfNeeded();

    void addSphere();
    void addMesh(const std::string filename);

    VKPT::SceneData sceneData;
    VKPT::StorageBuffer sceneStorage;
    std::vector<std::string> modelPaths;

  private:
    bool m_Reset = false;

    VkBuffer m_UniformBuffer;
    VkDeviceMemory m_UniformBufferMemory;
    void *m_UniformBufferMapped;

    bool m_StorageChanged = false;
    VkBuffer m_StorageBuffer;
    VkDeviceMemory m_StorageBufferMemory;
    void *m_StorageBufferMapped;
};

} // namespace Vulkan
