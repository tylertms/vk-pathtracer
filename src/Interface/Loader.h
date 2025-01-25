#pragma once

#include "../External/tiny_gltf/tiny_gltf.h"
#include "../Vulkan/Scene.h"

#include <string>

#include "glm/glm.hpp"

using namespace tinygltf;
namespace Interface {

class Loader {
  public:
    Loader(const std::string &filename);
    const std::vector<Vulkan::Triangle> &getTriangles() const { return m_Triangles; }
    const std::vector<Vulkan::Mesh> &getMeshes() const { return m_Meshes; }

  private:
    bool LoadModel(const std::string &filename);

    tinygltf::Model m_Model;
    std::string m_ErrorMsg;
    std::string m_WarningMsg;

    std::vector<Vulkan::Triangle> m_Triangles;
    std::vector<Vulkan::Mesh> m_Meshes;
};

} // namespace Interface
