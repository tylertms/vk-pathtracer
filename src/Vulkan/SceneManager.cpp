#include "SceneManager.h"
#include <vector>

namespace Vulkan {

/* ----------- FRAMES ----------- */
void SceneManager::setFramesRendered(uint32_t framesRendered) {
    m_Instance.framesRendered = framesRendered;
}

void SceneManager::incrementFramesRendered() {
    m_Instance.framesRendered++;
}
/* ----------------------------- */

/* ----------- RESET ----------- */
void SceneManager::resetAccumulation() {
    m_Reset = true;
    m_Instance.framesRendered = 0;
}

bool SceneManager::resetOccurred() {
    bool reset = m_Reset;
    m_Reset = false;
    return reset;
}
/* ----------------------------- */

/* ----------- CAMERA ----------- */
void SceneManager::setCam(const VKPT::Camera &cam) {
    m_Instance.camera = cam;
    resetAccumulation();
}
void SceneManager::setCamAspectRatio(float aspectRatio) {
    m_Instance.camera.aspectRatio = aspectRatio;
    resetAccumulation();
}
/* ----------------------------- */

/* ----------- SCENE ----------- */
void SceneManager::loadFromFile(const std::string filename) {
    if (filename.empty()) return;

    triangleBuffer.clear();
    modelPaths.clear();
    m_Instance.numTriangles = 0;
    m_Instance.numMeshes = 0;
    m_Instance.numSpheres = 0;

    Loader::loadSceneFromYAML(filename, modelPaths, m_Instance, triangleBuffer);

    for (uint32_t i = 0; i < m_Instance.numMeshes; i++) {
        applyMeshProperties(m_Instance.meshes[i]);
    }

    resetAccumulation();
}

void SceneManager::saveToFile(const std::string filename) {
    if (filename.empty()) return;
    Loader::saveSceneToYAML(filename, m_Instance, modelPaths);
}
/* ----------------------------- */

/* ----------- SPHERE ----------- */
void SceneManager::addSphere() {
    if (m_Instance.numSpheres == MAX_SPHERES) return;
    m_Instance.spheres[m_Instance.numSpheres++] = (VKPT::Sphere){};
    resetAccumulation();
}

void SceneManager::setNumSpheres(int numSpheres) {
    m_Instance.numSpheres = numSpheres;
}
/* ----------------------------- */

/* ----------- MESH ----------- */
void SceneManager::addMesh(const std::string filename) {
    if (filename.empty()) return;
    if (m_Instance.numMeshes == MAX_MESHES) return;

    Loader::GLTFLoader loader(filename);
    std::vector<VKPT::Triangle> triangles = loader.getTriangles();

    uint32_t start = m_Instance.numTriangles;
    uint32_t count = (uint32_t)triangles.size();
    if (start + count > MAX_TRIANGLES) return;

    VKPT::Mesh mesh;
    mesh.triangleCount = count;
    mesh.startIndex = start;

    triangleBuffer.reserve(start + count);
    for (uint32_t i = 0; i < count; i++) {
        m_Instance.triangles[start + i] = triangles[i];
        triangleBuffer.push_back(triangles[i]);
    }

    m_Instance.meshes[m_Instance.numMeshes++] = mesh;
    m_Instance.numTriangles += count;

    applyMeshProperties(mesh);
    modelPaths.push_back(filename);

    resetAccumulation();
}

void SceneManager::applyMeshProperties(VKPT::Mesh &mesh) {
    auto scale = mesh.scale;
    auto rotation = mesh.rotation;
    auto translation = mesh.translation;

    glm::mat4 transformation = createTransformationMatrix(scale, rotation, translation);

    glm::vec3 boundsMin = { FLT_MAX, FLT_MAX, FLT_MAX };
    glm::vec3 boundsMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

    for (uint32_t i = mesh.startIndex; i < mesh.startIndex + mesh.triangleCount; i++) {
        glm::vec3 _posA = applyTransformation(triangleBuffer[i].posA, transformation);
        glm::vec3 _posB = applyTransformation(triangleBuffer[i].posB, transformation);
        glm::vec3 _posC = applyTransformation(triangleBuffer[i].posC, transformation);
        glm::vec3 _normA = applyTransformation(triangleBuffer[i].normA, transformation);
        glm::vec3 _normB = applyTransformation(triangleBuffer[i].normB, transformation);
        glm::vec3 _normC = applyTransformation(triangleBuffer[i].normC, transformation);

        boundsMin = glm::min(boundsMin, glm::min(_posA, glm::min(_posB, _posC)));
        boundsMax = glm::max(boundsMax, glm::max(_posA, glm::max(_posB, _posC)));

        m_Instance.triangles[i].posA = _posA;
        m_Instance.triangles[i].posB = _posB;
        m_Instance.triangles[i].posC = _posC;
        m_Instance.triangles[i].normA = _normA;
        m_Instance.triangles[i].normB = _normB;
        m_Instance.triangles[i].normC = _normC;
    }

    mesh.bounds.min = boundsMin;
    mesh.bounds.max = boundsMax;

    resetAccumulation();
}
/* ----------------------------- */

} // namespace Vulkan
