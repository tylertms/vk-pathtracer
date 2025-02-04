#pragma once

#include <cstring>
#include <iostream>
#include <stdint.h>
#include <vector>

#include "Device.h"
#include "VulkanApp.h"
#include "vulkan/vulkan_core.h"

#include "../Core/IO/GLTFLoader.h"
#include "../Core/Types/Scene.h"
#include "../Core/Utils/Matrix.h"

namespace Vulkan {

class SceneManager {
  public:
    NO_COPY(SceneManager)
    SceneManager() = default;

    void init(const Device &device, const CommandPool &commandPool);
    void deinit(const VkDevice &device);
    void reset();

    inline const VkBuffer &getUniformBuffer() const { return m_UniformBuffer; }
    inline const VkBuffer &getSceneStorage() const { return m_SceneStorage; }

    inline const VkImageView &getEnvImageView() const { return m_TextureEnv.getVkImageView(); }
    inline const VkSampler &getEnvSampler() const { return m_TextureEnv.getVkSampler(); }

    void resetAccumulation();
    bool resetOccurred();
    void submitUniformUpdates();
    void uploadFullSceneStorage();

    void addSphere();
    void addMesh(const std::string filename);
    void updateMeshTransforms();

    VKPT::SceneData sceneData;
    VKPT::SceneStorage *sceneStorage;

    std::vector<std::string> modelPaths;
    std::vector<glm::mat3> meshTransforms;

  private:
    VkBuffer m_UniformBuffer;
    VkDeviceMemory m_UniformBufferMemory;
    void *m_UniformBufferMapped;

    VkBuffer m_SceneStorage;
    VkDeviceMemory m_SceneStorageMemory;
    void *m_SceneStorageMapped;

    ImageView m_TextureEnv;
};

} // namespace Vulkan
