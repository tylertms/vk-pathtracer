#pragma once

#include <cstring>
#include <iostream>
#include <stdint.h>
#include <vector>

#include "Device.h"
#include "VulkanApp.h"
#include "vulkan/vulkan_core.h"

#include "../Core/Types/Scene.h"
#include "../Core/Utils/Matrix.h"
#include "../Core/IO/GLTFLoader.h"

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
    void addMesh(const std::string filename, const glm::mat3 transform);

    VKPT::SceneData sceneData;
    VKPT::StorageBuffer sceneStorage;

    std::vector<std::string> modelPaths;
    std::vector<glm::mat3> meshTransforms;

  private:
    bool m_Reset = false;

    VkBuffer m_UniformBuffer;
    VkDeviceMemory m_UniformBufferMemory;
    void *m_UniformBufferMapped;

    VkBuffer m_StorageBuffer;
    VkDeviceMemory m_StorageBufferMemory;
    void *m_StorageBufferMapped;
};

} // namespace Vulkan
