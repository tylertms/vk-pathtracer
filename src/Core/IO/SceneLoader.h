#pragma once

#include <string>

#include "../Types/Scene.h"
#include "../../Vulkan/SceneManager.h"

namespace IO {

std::string extractDirectory(const std::string& filepath);

void loadSceneFromYAML(const std::string filename, Vulkan::SceneManager &sceneManager);
void saveSceneToYAML(const std::string filename, const Vulkan::SceneManager &sceneManager);

}
