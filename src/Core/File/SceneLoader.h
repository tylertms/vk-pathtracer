#pragma once

#include <string>

#include "../../Vulkan/SceneManager.h"
#include "../Types/Scene.h"

namespace File {

std::string extractDirectory(const std::string &filepath);

void loadSceneFromYAML(const std::string filename, Vulkan::SceneManager &sceneManager);
void saveSceneToYAML(const std::string filename, const Vulkan::SceneManager &sceneManager);

} // namespace File
