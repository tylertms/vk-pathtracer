#pragma once

#include "../../Vulkan/SceneManager.h"
#include "../Types/BVH.h"
#include <cfloat>

namespace BVH {

static std::mutex bvhAllocationMutex;

void createBVH(VKPT::Mesh &mesh, VKPT::BVH &rootBVH, Vulkan::SceneManager &sceneManager);
void splitBVH(Vulkan::SceneManager &sceneManager, uint32_t bvhIndex, uint8_t depth = 0);

} // namespace BVH
