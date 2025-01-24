#pragma once

#define MAX_SPHERES 8

struct Camera {
    float lookFrom[3];
    float vfov;
    float lookAt[3];
    float aspectRatio;
};

struct Material {
    float color[3];
    float emissionStrength;
    float emissionColor[3];
    float padding;
};

struct Sphere {
    float center[3];
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

    void incrementFramesRendered() {
        m_Instance.framesRendered++;
    }

    void resetAccumulation() {
        m_Instance.framesRendered = -1;
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
