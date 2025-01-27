#pragma once

#include <cstring>
#include <iostream>
#include <stdint.h>
#include <vector>

#include "../Core/Types/SceneObject.h"
#include "../Core/Utils/Matrix.h"
#include "../Core/Loader/SceneLoader.h"

namespace Vulkan {

class SceneManager {
  public:
    void setFramesRendered(uint32_t framesRendered);
    void incrementFramesRendered();

    void resetAccumulation();
    bool resetOccurred();

    void setCam(const VKPT::Camera &cam);
    void setCamAspectRatio(float aspectRatio);

    void loadFromFile(const std::string filename);
    void saveToFile(const std::string filename);

    void addSphere();
    void setNumSpheres(int numSpheres);

    void addMesh(const std::vector<VKPT::Triangle> &triangles);
    void applyMeshProperties(VKPT::Mesh &mesh);

    inline VKPT::SceneObject *getObject() { return &m_Instance; }

    std::vector<std::string> modelPaths;
    std::vector<VKPT::Triangle> triangleBuffer;

  private:
    VKPT::SceneObject m_Instance;
    bool m_Reset = true;
};

} // namespace Vulkan
