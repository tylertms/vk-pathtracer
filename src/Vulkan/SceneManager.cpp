#include "SceneManager.h"
#include "vulkan/vulkan_core.h"
#include <vector>

namespace Vulkan {

/* ----------- INIT ----------- */
void SceneManager::init(const Device &device) {
    sceneUniform.init(device);

    createBuffer(device, sizeof(VKPT::StorageBuffer), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Buffer, m_BufferMemory);
    vkMapMemory(device.getVkDevice(), m_BufferMemory, 0, sizeof(VKPT::StorageBuffer), 0, &m_BufferMapped);
}

void SceneManager::deinit(const VkDevice &device) {
    sceneUniform.deinit(device);

    vkDestroyBuffer(device, m_Buffer, nullptr);
    vkFreeMemory(device, m_BufferMemory, nullptr);
}
/* ----------------------------- */

/* ----------- RESET ----------- */
void SceneManager::resetAccumulation() {
    m_Reset = true;
    sceneUniform.setFramesRendered(0);
}

bool SceneManager::resetOccurred() {
    bool reset = m_Reset;
    m_Reset = false;
    return reset;
}
/* ----------------------------- */


/* ----------- SPHERE ----------- */
void SceneManager::addSphere() {
    sceneUniform.data.numSpheres++;
}

/* ----------------------------- */

/* ----------- MESH ----------- */
void SceneManager::addMesh(const std::string filename) {
    if (filename.empty()) return;
    Loader::GLTFLoader loader(filename);

    for (auto &triangle : loader.getTriangles()) {
        sceneStorage.triangles[sceneUniform.data.numTriangles++] = triangle;
    }

    for (auto &mesh : loader.getMeshes()) {
        sceneStorage.meshes[sceneUniform.data.numMeshes++] = mesh;
    }

    modelPaths.push_back(filename);
}

/* ----------------------------- */

} // namespace Vulkan
