#include "SceneManager.h"
#include "vulkan/vulkan_core.h"
#include <cfloat>
#include <cstring>
#include <vector>

namespace Vulkan {

/* ----------- INIT ----------- */
void SceneManager::init(const Device &device) {
    sceneStorage = new VKPT::SceneStorage;

    createBuffer(device, sizeof(VKPT::SceneData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UniformBuffer, m_UniformBufferMemory);
    vkMapMemory(device.getVkDevice(), m_UniformBufferMemory, 0, sizeof(VKPT::SceneData), 0, &m_UniformBufferMapped);

    createBuffer(device, sizeof(VKPT::SceneStorage), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_SceneStorage, m_SceneStorageMemory);
    vkMapMemory(device.getVkDevice(), m_SceneStorageMemory, 0, sizeof(VKPT::SceneStorage), 0, &m_SceneStorageMapped);
}

void SceneManager::deinit(const VkDevice &device) {
    vkDestroyBuffer(device, m_UniformBuffer, nullptr);
    vkFreeMemory(device, m_UniformBufferMemory, nullptr);

    vkDestroyBuffer(device, m_SceneStorage, nullptr);
    vkFreeMemory(device, m_SceneStorageMemory, nullptr);

    delete sceneStorage;
}

void SceneManager::reset() {
    sceneData.numMeshes = 0;
    sceneData.numSpheres = 0;
    sceneData.numTriangles = 0;
    sceneData.framesRendered = 0;
    modelPaths.clear();
    meshTransforms.clear();
}
/* ----------------------------- */

/* ----------- MEMORY ----------- */
void SceneManager::submitUniformUpdates() {
    memcpy(m_UniformBufferMapped, &sceneData, sizeof(VKPT::SceneData));
}

void SceneManager::uploadFullSceneStorage() {
    printf("UPDATING FULL SCENE STORAGE\n");
    memcpy(m_SceneStorageMapped, sceneStorage, sizeof(VKPT::SceneStorage));
}
/* ----------------------------- */

/* ----------- RESET ----------- */
void SceneManager::resetAccumulation() {
    sceneData.framesRendered = 0;
}
/* ----------------------------- */

/* ----------- SPHERE ----------- */
void SceneManager::addSphere() {
    sceneData.numSpheres++;
}
/* ----------------------------- */

/* ----------- MESH ----------- */
void SceneManager::addMesh(const std::string filename) {
    if (filename.empty()) return;
    IO::GLTFLoader loader(filename);

    uint32_t triStartIndex = sceneData.numTriangles;

    for (auto &triangle : loader.getTriangles()) {
        sceneStorage->triangles[sceneData.numTriangles++] = triangle;
    }

    for (auto mesh : loader.getMeshes()) {
        mesh.startIndex = triStartIndex;
        growBoundingBox(mesh);
        sceneData.meshes[sceneData.numMeshes++] = mesh;
    }

    glm::mat3 defaultTransform = 0;
    defaultTransform[2] = { 1, 1, 1 };

    meshTransforms.push_back(defaultTransform);
    modelPaths.push_back(filename);
}

void SceneManager::updateMeshTransforms() {
    for (uint32_t i = 0; i < sceneData.numMeshes; i++) {
        auto &worldLocalTransform = sceneData.meshes[i].worldLocalTransform;
        auto &localWorldTransform = sceneData.meshes[i].localWorldTransform;
        VKPT::computeInverseMatrix(worldLocalTransform, localWorldTransform, meshTransforms[i]);
    }
}

void SceneManager::growBoundingBox(VKPT::Mesh &mesh) {
    vec3 bMin = { FLT_MAX, FLT_MAX, FLT_MAX };
    vec3 bMax = -bMin;

    for (int i = mesh.startIndex; i < mesh.startIndex + mesh.triangleCount; i++) {
        VKPT::Triangle tri = sceneStorage->triangles[i];
        bMin = min(bMin, min(tri.posA, min(tri.posB, tri.posC)));
        bMax = max(bMax, max(tri.posA, max(tri.posB, tri.posC)));
    }

    mesh.bounds.min = bMin;
    mesh.bounds.max = bMax;
}
/* ----------------------------- */

} // namespace Vulkan
