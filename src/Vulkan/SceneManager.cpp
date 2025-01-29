#include "SceneManager.h"
#include "vulkan/vulkan_core.h"
#include <vector>

namespace Vulkan {

/* ----------- INIT ----------- */
void SceneManager::init(const Device &device) {
    createBuffer(device, sizeof(VKPT::SceneData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBuffer, m_UniformBufferMemory);
    vkMapMemory(device.getVkDevice(), m_UniformBufferMemory, 0, sizeof(VKPT::SceneData), 0, &m_UniformBufferMapped);

    createBuffer(device, sizeof(VKPT::StorageBuffer), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_StorageBuffer, m_StorageBufferMemory);
    vkMapMemory(device.getVkDevice(), m_StorageBufferMemory, 0, sizeof(VKPT::StorageBuffer), 0, &m_StorageBufferMapped);

}

void SceneManager::deinit(const VkDevice &device) {
    vkDestroyBuffer(device, m_UniformBuffer, nullptr);
    vkFreeMemory(device, m_UniformBufferMemory, nullptr);

    vkDestroyBuffer(device, m_StorageBuffer, nullptr);
    vkFreeMemory(device, m_StorageBufferMemory, nullptr);
}

void SceneManager::reset() {
    sceneData.numMeshes = 0;
    sceneData.numSpheres = 0;
    modelPaths.clear();
    meshTransforms.clear();
}
/* ----------------------------- */

/* ----------- MEMORY ----------- */
void SceneManager::submitUniformUpdates() {
    memcpy(m_UniformBufferMapped, &sceneData, sizeof(sceneData));
}

void SceneManager::submitStorageUpdatesIfNeeded() {
    if (!m_Reset) return;
    m_Reset = false;
    memcpy(m_StorageBufferMapped, &sceneStorage, sizeof(sceneStorage));
}
/* ----------------------------- */

/* ----------- RESET ----------- */
void SceneManager::resetAccumulation() {
    m_Reset = true;
    sceneData.framesRendered = 0;


    for (uint32_t i = 0; i < sceneData.numMeshes; i++) {
        sceneStorage.meshes[i].invTransform = VKPT::computeInverseMatrix(meshTransforms[i]);
    }
}

bool SceneManager::resetOccurred() {
    bool reset = m_Reset;
    m_Reset = false;
    return reset;
}
/* ----------------------------- */


/* ----------- SPHERE ----------- */
void SceneManager::addSphere() {
    sceneData.numSpheres++;
    m_Reset = true;
}
/* ----------------------------- */

/* ----------- MESH ----------- */
void SceneManager::addMesh(const std::string filename) {
    if (filename.empty()) return;
    Loader::GLTFLoader loader(filename);

    for (auto &triangle : loader.getTriangles()) {
        sceneStorage.triangles[sceneData.numTriangles++] = triangle;
    }

    for (auto &mesh : loader.getMeshes()) {
        sceneStorage.meshes[sceneData.numMeshes++] = mesh;
    }

    glm::mat3 defaultTransform = 0;
    defaultTransform[2] = { 1, 1, 1 };

    meshTransforms.push_back(defaultTransform);
    modelPaths.push_back(filename);
    m_Reset = true;
}
/* ----------------------------- */

} // namespace Vulkan
