#pragma once

#include "../Types/BVH.h"
#include "../../Vulkan/SceneManager.h"
#include <cfloat>

namespace BVH {

void createBVH(VKPT::Mesh &mesh, VKPT::BVH &rootBVH, Vulkan::SceneManager &sceneManager);
void splitBVH(Vulkan::SceneManager &sceneManager, uint32_t bvhIndex, uint8_t depth = 0);

}
