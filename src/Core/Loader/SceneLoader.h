#pragma once

#include <string>
#include "../Types/SceneObject.h"

namespace Loader {

void loadSceneFromYAML(const std::string &filename, std::vector<std::string> &modelPaths, VKPT::SceneObject &sceneObj, std::vector<VKPT::Triangle> &triangleBuffer);
void saveSceneToYAML(const std::string &filename, VKPT::SceneObject &sceneObj, const std::vector<std::string> &modelPaths);

}
