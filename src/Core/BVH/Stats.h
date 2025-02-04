#pragma once

#include "../../Vulkan/SceneManager.h"

namespace BVH {

void collectAndPrintStats(Vulkan::SceneManager &sceneManager, uint32_t rootIndex, float buildTime);

}
