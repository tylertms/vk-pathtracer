#pragma once

#include <cstring>
#include <stdint.h>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdio>

#define MAX_SPHERES 4
#define MAX_TRIANGLES 2000
#define MAX_MESHES 4

namespace Vulkan {

struct Camera {
    float lookFrom[3];
    float vfov;
    float lookAt[3];
    float aspectRatio;
};

struct Material {
    float color[3] = { 1.0, 1.0, 1.0 };
    float emissionStrength = 0.0;
    float emissionColor[3] = { 1.0, 1.0, 1.0 };
    float padding;
};

struct Sphere {
    float center[3] = { 0.0, 0.0, 0.0 };
    float radius = 0.5;
    Material material;
};

struct Triangle {
    float posA[4];
    float posB[4];
    float posC[4];
    float normA[4];
    float normB[4];
    float normC[4];
};

struct Mesh {
    float translation[3] = { 0.0, 0.0, 0.0 };
    uint32_t triangleCount;
    float scale[3] = { 1.0, 1.0, 1.0 };
    uint32_t startIndex;
    float rotation[3] = { 0.0, 0.0, 0.0 };
    uint32_t padding;
    Material material;
};

struct SceneObject {
    Camera cam;
    Sphere spheres[MAX_SPHERES];
    Triangle triangles[MAX_TRIANGLES];
    Mesh meshes[MAX_MESHES];
    uint32_t numSpheres = 0;
    uint32_t numTriangles = 0;
    uint32_t numMeshes = 0;
    uint32_t framesRendered = 0;
};

class Scene {
  public:
    void setFramesRendered(uint32_t framesRendered) {
        m_Instance.framesRendered = framesRendered;
    }

    void incrementFramesRendered() {
        m_Instance.framesRendered++;
    }

    void resetAccumulation() {
        m_Reset = true;
        m_Instance.framesRendered = 0;
    }

    bool resetOccurred() {
        bool reset = m_Reset;
        m_Reset = false;
        return reset;
    }

    void setCam(const Camera &cam) { m_Instance.cam = cam; }
    void setCamAspectRatio(float aspectRatio) { m_Instance.cam.aspectRatio = aspectRatio; }

    void addSphere() {
        if (m_Instance.numSpheres == MAX_SPHERES) return;
        m_Instance.spheres[m_Instance.numSpheres++] = (Sphere){};
        resetAccumulation();
    }

    void addMesh(const std::vector<Triangle> &ts) {
        if (m_Instance.numMeshes == MAX_MESHES) return;

        uint32_t start = m_Instance.numTriangles;
        uint32_t count = (uint32_t)ts.size();
        if (start + count > MAX_TRIANGLES) return;

        Mesh mesh;
        mesh.triangleCount = count;
        mesh.startIndex = start;

        m_Instance.meshes[m_Instance.numMeshes++] = mesh;
        m_Instance.numTriangles += count;

        for (uint32_t i = 0; i < count; i++) {
            m_Instance.triangles[start + i] = ts[i];
        }

        //memcpy(m_Instance.triangles + start, ts.data(), count * sizeof(Triangle));

        resetAccumulation();
    }

    glm::mat4 createTransformationMatrix(
        const glm::vec3& scaleFactors,
        const glm::vec3& rotationDegrees,
        const glm::vec3& translation
    ) {
        glm::mat4 transform = glm::mat4(1.0f);

        transform = glm::scale(transform, scaleFactors);

        transform = glm::rotate(transform, glm::radians(rotationDegrees.x), glm::vec3(1, 0, 0));
        transform = glm::rotate(transform, glm::radians(rotationDegrees.y), glm::vec3(0, 1, 0));
        transform = glm::rotate(transform, glm::radians(rotationDegrees.z), glm::vec3(0, 0, 1));

        transform = glm::translate(transform, translation);

        return transform;
    }

    glm::vec3 applyTransformation(
        const glm::vec3& vec,
        const glm::mat4& transform
    ) {
        glm::vec4 homogenousVec = glm::vec4(vec, 1.0f);
        glm::vec4 transformedVec = transform * homogenousVec;
        return glm::vec3(transformedVec);
    }

    void updateMesh(Mesh mesh) {
        auto scale = glm::make_vec3(mesh.scale);
        auto rotation = glm::make_vec3(mesh.rotation);
        auto translation = glm::make_vec3(mesh.translation);

        glm::mat4 transformation = createTransformationMatrix(scale, rotation, translation);

        for (uint32_t i = mesh.startIndex; i < mesh.startIndex + mesh.triangleCount; i++) {
            glm::vec3 _posA = applyTransformation(glm::make_vec3(m_Instance.triangles[i].posA), transformation);
            glm::vec3 _posB = applyTransformation(glm::make_vec3(m_Instance.triangles[i].posB), transformation);
            glm::vec3 _posC = applyTransformation(glm::make_vec3(m_Instance.triangles[i].posC), transformation);
            glm::vec3 _normA = applyTransformation(glm::make_vec3(m_Instance.triangles[i].normA), transformation);
            glm::vec3 _normB = applyTransformation(glm::make_vec3(m_Instance.triangles[i].normB), transformation);
            glm::vec3 _normC = applyTransformation(glm::make_vec3(m_Instance.triangles[i].normC), transformation);

            memcpy(m_Instance.triangles[i].posA, &_posA, 12);
            memcpy(m_Instance.triangles[i].posB, &_posB, 12);
            memcpy(m_Instance.triangles[i].posC, &_posC, 12);
            memcpy(m_Instance.triangles[i].normA, &_normA, 12);
            memcpy(m_Instance.triangles[i].normB, &_normB, 12);
            memcpy(m_Instance.triangles[i].normC, &_normC, 12);
        }

        resetAccumulation();
    }

    void setNumSpheres(int numSpheres) {
        m_Instance.numSpheres = numSpheres;
    }

    inline SceneObject *getObject() { return &m_Instance; }

  private:
    SceneObject m_Instance;
    bool m_Reset = true;
};

} // namespace Vulkan
