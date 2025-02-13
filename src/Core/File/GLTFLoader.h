#pragma once

#include "FilePicker.h"

#include "../Types/Scene.h"
#include "../../Vulkan/SceneManager.h"
#include "../../External/tiny_gltf/tiny_gltf.h"

#include <glm/glm.hpp>
#include <string>

using namespace tinygltf;
namespace File {

void loadGLTF(const std::string &filename, const glm::mat3 &transform, uint32_t matIndex, Vulkan::SceneManager &sceneManager);

} // namespace File
