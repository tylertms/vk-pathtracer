#pragma once

#include "../Types/BVH.h"
#include "../../Vulkan/SceneManager.h"
#include <cfloat>

namespace BVH {

struct SplitData {
    uint32_t axis = 0;
    float pos = 0;
    float cost = FLT_MAX;
};

void createBVH(VKPT::Mesh &mesh, VKPT::BVH &rootBVH, Vulkan::SceneManager &sceneManager);

}
