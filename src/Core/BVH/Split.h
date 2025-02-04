#pragma once

#include "../../Vulkan/SceneManager.h"

namespace BVH {

struct SplitData {
    uint32_t axis = 0;
    float pos = 0;
    float cost = FLT_MAX;
};

float evaluateSplit(const Vulkan::SceneManager &sceneManager, const VKPT::BVH &bvh, int splitAxis, float splitPos);
SplitData chooseSplit(const Vulkan::SceneManager &sceneManager, const VKPT::BVH &bvh);;

}
