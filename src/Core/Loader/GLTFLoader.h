#pragma once

#include "../../External/tiny_gltf/tiny_gltf.h"
#include "../Types/SceneObject.h"

#include <string>
#include <glm/glm.hpp>

using namespace tinygltf;
namespace Loader {

class GLTFLoader {
  public:
    GLTFLoader(const std::string &filename);
    const std::vector<VKPT::Triangle> &getTriangles() const { return m_Triangles; }
    const std::vector<VKPT::Mesh> &getMeshes() const { return m_Meshes; }

  private:
    bool LoadModel(const std::string &filename);

    tinygltf::Model m_Model;
    std::string m_ErrorMsg;
    std::string m_WarningMsg;

    std::vector<VKPT::Triangle> m_Triangles;
    std::vector<VKPT::Mesh> m_Meshes;
};

} // namespace Loader
