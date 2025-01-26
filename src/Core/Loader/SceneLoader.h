#pragma once

#include <string>
#include "../Types/SceneObject.h"

namespace Loader {

void loadSceneFromYAML(const std::string &filename, VKPT::SceneObject &sceneObj, std::vector<VKPT::Triangle> &triangles);

}
