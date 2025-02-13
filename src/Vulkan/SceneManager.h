#pragma once

#include <cstring>
#include <iostream>
#include <stdint.h>
#include <vector>

#include "Device.h"
#include "ImageView.h"
#include "VulkanApp.h"
#include "vulkan/vulkan_core.h"

#include "../External/tiny_gltf/tiny_gltf.h"

#include "../Core/Types/Scene.h"
#include "../Core/Utils/Matrix.h"
#include "../Core/File/FilePicker.h"

namespace Vulkan {

class SceneManager {
  public:
    NO_COPY(SceneManager)
    SceneManager() = default;

    void init(const Device &device, const VkExtent2D extent, const CommandPool &commandPool);
    void deinit(const VkDevice &device);
    void reset();

    inline const VkBuffer &getUniformBuffer() const { return m_UniformBuffer; }
    inline const VkBuffer &getSceneStorage() const { return m_SceneStorage; }

    void resetAccumulation();
    void submitUniformUpdates();
    void uploadFullSceneStorage();

    void addSphere();
    void addCamera(std::string name, tinygltf::Camera camera, glm::mat3 &transform);
    //void addMesh(const std::string filename, glm::mat3 transform = glm::mat3(0), uint32_t matIndex = -1);

    void updateMeshTransforms();

    void updateEnvTexture(const std::string filename);
    void loadTexture(const std::string filename, uint32_t &textureIndex);

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

    std::vector<VKPT::Camera> cameras;
    std::vector<std::string> cameraNames;

    bool texturesUpdated = false;
    ImageView accumulationImageView;
    ImageView textures[MAX_TEXTURES];
    std::vector<std::string> texturePaths;

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
