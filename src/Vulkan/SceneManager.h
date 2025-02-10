#pragma once

#include <cstring>
#include <iostream>
#include <stdint.h>
#include <vector>

#include "Device.h"
#include "ImageView.h"
#include "VulkanApp.h"
#include "vulkan/vulkan_core.h"

#include "../Core/File/GLTFLoader.h"
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

    void resetAccumulation();
    void submitUniformUpdates();
    void uploadFullSceneStorage();

    void addSphere();
    void addMesh(const std::string filename, glm::mat3 transform = glm::mat3(0), uint32_t matIndex = -1);

    void queueEnv(const std::string filename);
    void loadEnv();

    void updateMeshTransforms();

    VKPT::SceneData sceneData;
    VKPT::SceneStorage *sceneStorage;

    uint32_t selectedObjectIndex = -1;
    VKPT_FILE_TYPE selectedObjectType;

    std::vector<std::string> modelPaths;
    std::vector<glm::mat3> meshTransforms;

    std::vector<uint32_t> triIndices;
    std::vector<glm::vec3> triMin;
    std::vector<glm::vec3> triMax;
    std::vector<glm::vec3> triCentroid;

    ImageView envTexture;
    std::string texturePath;
    bool updateTexture = false;

  private:
    VkBuffer m_UniformBuffer;
    VkDeviceMemory m_UniformBufferMemory;
    void *m_UniformBufferMapped;

    VkBuffer m_SceneStorage;
    VkDeviceMemory m_SceneStorageMemory;

    const Device *ext_Device;
    const CommandPool *ext_CommandPool;
};

} // namespace Vulkan
