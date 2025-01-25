#pragma once

#include <cstring>
#include <stdint.h>
#include <vector>

#define MAX_SPHERES 128
#define MAX_TRIANGLES 10000
#define MAX_MESHES 128

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
    uint32_t triangleCount;
    uint32_t startIndex;
    uint32_t padding[2];
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
        m_Instance.framesRendered = -1;
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
        if (start + count >= MAX_TRIANGLES) return;

        Mesh mesh;
        mesh.triangleCount = count;
        mesh.startIndex = start;

        m_Instance.meshes[m_Instance.numMeshes++] = mesh;
        m_Instance.numTriangles += count;

        for (int i = 0; i < count; i++) {
            m_Instance.triangles[start + i] = ts[i];
        }

        //memcpy(m_Instance.triangles + start, ts.data(), count * sizeof(Triangle));

        resetAccumulation();
    }

    void setNumSpheres(int numSpheres) {
        m_Instance.numSpheres = numSpheres;
    }

    inline SceneObject *getObject() { return &m_Instance; }

  private:
    SceneObject m_Instance;
};

} // namespace Vulkan
