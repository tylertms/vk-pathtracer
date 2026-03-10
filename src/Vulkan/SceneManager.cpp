#include "SceneManager.h"

#include "ImageView.h"
#include "Texture.h"
#include "vulkan/vulkan_core.h"

#include <cstring>
#include <vector>

namespace Vulkan {

static void reloadTextureSlot(ImageView &texture, const std::string &filename, const Device &device, const CommandPool &commandPool) {
    if (texture.getVkImageView() != nullptr || texture.getVkImage() != nullptr || texture.getVkSampler() != nullptr) {
        texture.deinit(device.getVkDevice());
    }
    createTextureImage(filename, texture, device, commandPool);
}

/* ----------- INIT ----------- */
void SceneManager::init(const Device &device, const VkExtent2D extent, const CommandPool &commandPool) {
    ext_Device = &device;
    ext_CommandPool = &commandPool;

    sceneStorage = new VKPT::SceneStorage();

    createBuffer(device, sizeof(VKPT::SceneData), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_UniformBuffer, m_UniformBufferMemory);
    vkMapMemory(device.getVkDevice(), m_UniformBufferMemory, 0, sizeof(VKPT::SceneData), 0, &m_UniformBufferMapped);

    createBuffer(device, sizeof(VKPT::SceneStorage), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_SceneStorage, m_SceneStorageMemory);

    for (uint32_t i = 0; i < MAX_TEXTURES; i++) {
        VkFormat format = i == 0 ? VK_FORMAT_R32G32B32A32_SFLOAT : VK_FORMAT_R8G8B8A8_UNORM;
        textures[i].createImage(device, {1, 1}, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        textures[i].init(device.getVkDevice(), nullptr, format);
        textures[i].transitionImageLayout(device, commandPool, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    textures[0].createSampler(device);

    accumulationImageView.createImage(device, extent, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_STORAGE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    accumulationImageView.transitionImageLayout(device, commandPool, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
    accumulationImageView.init(device.getVkDevice(), nullptr, VK_FORMAT_UNDEFINED);
}

void SceneManager::deinit(const VkDevice &device) {
    vkDestroyBuffer(device, m_UniformBuffer, nullptr);
    vkFreeMemory(device, m_UniformBufferMemory, nullptr);

    vkDestroyBuffer(device, m_SceneStorage, nullptr);
    vkFreeMemory(device, m_SceneStorageMemory, nullptr);

    for (uint32_t i = 0; i < MAX_TEXTURES; i++) {
        textures[i].deinit(device);
    }

    accumulationImageView.deinit(device);
    delete sceneStorage;
}

void SceneManager::reset() {
    const glm::uvec2 windowSize = sceneData.camera.windowSize;
    sceneData = {};
    sceneData.camera.windowSize = windowSize;
    selectedObjectIndex = static_cast<uint32_t>(-1);
    modelPaths.clear();
    meshNames.clear();
    texturePaths.clear();
    meshTransforms.clear();
    triIndices.clear();
    triMin.clear();
    triMax.clear();
    triCentroid.clear();
    cameras.clear();
    cameraNames.clear();

    reloadTextureSlot(textures[0], "", *ext_Device, *ext_CommandPool);
    texturesUpdated = true;
}
/* ----------------------------- */

/* ----------- MEMORY ----------- */
void SceneManager::submitUniformUpdates() {
    memcpy(m_UniformBufferMapped, &sceneData, sizeof(VKPT::SceneData));
}

void SceneManager::uploadFullSceneStorage() {
    printf("UPLOADING FULL SCENE\n");
    const Device &device = *ext_Device;
    const VkDevice &vkDevice = device.getVkDevice();

    VkDeviceSize storageSize = sizeof(VKPT::SceneStorage);

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(device, storageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(vkDevice, stagingBufferMemory, 0, storageSize, 0, &data);
    memcpy(data, sceneStorage, storageSize);
    vkUnmapMemory(vkDevice, stagingBufferMemory);

    ext_CommandPool->copyBuffer(device, stagingBuffer, m_SceneStorage, storageSize);

    vkDestroyBuffer(vkDevice, stagingBuffer, nullptr);
    vkFreeMemory(vkDevice, stagingBufferMemory, nullptr);
}

/* ----------------------------- */

/* ----------- RESET ----------- */
void SceneManager::resetAccumulation() {
    sceneData.framesRendered = 0;
}
/* ----------------------------- */

/* ----------- SPHERE ----------- */
void SceneManager::addSphere() {
    sceneData.spheres[sceneData.numSpheres].materialIndex = sceneData.numMaterials++;
    selectedObjectIndex = sceneData.numSpheres++;
    selectedObjectType = VKPT_SPHERE;

    resetAccumulation();
}
/* ----------------------------- */

/* ----------- MESH ----------- */
void SceneManager::updateMeshTransforms() {
    for (uint32_t i = 0; i < sceneData.numMeshes; i++) {
        auto &worldLocalTransform = sceneData.meshes[i].worldLocalTransform;
        auto &localWorldTransform = sceneData.meshes[i].localWorldTransform;
        VKPT::computeInverseMatrix(worldLocalTransform, localWorldTransform, meshTransforms[i]);
    }
}
/* ----------------------------- */

/* ----------- TEXTURE ----------- */
void SceneManager::updateEnvTexture(const std::string filename) {
    if (texturePaths.size() < 1) {
        texturePaths.resize(1);
    }
    texturePaths[0] = filename;
    reloadTextureSlot(textures[0], filename, *ext_Device, *ext_CommandPool);
    texturesUpdated = true;
    resetAccumulation();
}

void SceneManager::loadTexture(const std::string filename, const uint32_t textureIndex) {
    if (filename.empty())
        return;

    if (texturePaths.size() <= textureIndex) {
        texturePaths.resize(textureIndex + 1);
    }
    texturePaths[textureIndex] = filename;
    reloadTextureSlot(textures[textureIndex], filename, *ext_Device, *ext_CommandPool);
    texturesUpdated = true;
    resetAccumulation();
}

/* ----------------------------- */

void SceneManager::addCamera(std::string name, tinygltf::Camera camera, glm::mat3 &transform) {
    cameraNames.push_back(name);
    VKPT::Camera cam;
    cam.vfov = camera.perspective.yfov;
    cam.lookFrom = transform[0];
    cam.lookAt = transform[1];
    cameras.push_back(cam);
}

} // namespace Vulkan
