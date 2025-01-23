#pragma once

#include "glm/glm.hpp"

#define MAX_SPHERES 8

struct Camera {
    glm::vec3 lookFrom;
    float vfov;
    glm::vec3 lookAt;
    float aspectRatio;
};

struct Material {
    glm::vec3 color;
    float emissionStrength;
    glm::vec3 emissionColor;
    float padding;
};

struct Sphere {
    glm::vec3 center;
    float radius;
    Material material;
};

struct SceneObject {
    Camera cam;
    Sphere spheres[MAX_SPHERES];
    uint32_t numSpheres;
    uint32_t framesRendered;
};

namespace Vulkan {

class Scene {
  public:
    void setFramesRendered(uint32_t framesRendered) {
        m_Instance.framesRendered = framesRendered;
    }

    void setCam(const Camera &cam) { m_Instance.cam = cam; }
    void setCamAspectRatio(float aspectRatio) { m_Instance.cam.aspectRatio = aspectRatio; }

    void addSphere(const Sphere sphere) {
        m_Instance.spheres[m_Instance.numSpheres++] = sphere;
    }

    void setNumSpheres(int numSpheres) {
        m_Instance.numSpheres = numSpheres;
    }

    inline SceneObject *getObject() { return &m_Instance; }

  private:
    SceneObject m_Instance;
};

} // namespace Vulkan
