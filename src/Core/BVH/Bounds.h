#pragma once

#include "../../Vulkan/SceneManager.h"

#include "glm/glm.hpp"

namespace BVH {

inline void growBounds(glm::vec3 &boundsMin, glm::vec3 &boundsMax, const glm::vec3 &triMin, const glm::vec3 &triMax) {
    boundsMin = glm::min(boundsMin, triMin);
    boundsMax = glm::max(boundsMax, triMax);
}

inline float nodeCost(const glm::vec3 &boundsMin, const glm::vec3 &boundsMax, uint32_t triangleCount) {
    glm::vec3 size = boundsMax - boundsMin;
    float halfArea = size.x * (size.y + size.z) + size.y * size.z;
    return halfArea * triangleCount;
}

void computeBounds(const Vulkan::SceneManager &sceneManager, VKPT::BVH &bvh);

} // namespace BVH
