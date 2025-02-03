#pragma once

#include "../Types/BVH.h"
#include "../../Vulkan/SceneManager.h"

namespace BVH {

void createBVH(const Vulkan::SceneManager &sceneManager);

}
