#pragma once

#include "../../External/tiny_gltf/tiny_gltf.h"
#include "../Types/Scene.h"
#include "FilePicker.h"

#include <glm/glm.hpp>
#include <string>

using namespace tinygltf;
namespace IO {

class GLTFLoader {
  public:
    GLTFLoader(VKPT_FILE_TYPE type);
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

} // namespace IO
