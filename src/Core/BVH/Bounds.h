#pragma once

#include "../../Vulkan/SceneManager.h"

#include "glm/glm.hpp"

namespace BVH {

inline void growBounds(glm::vec3 &boundsMin, glm::vec3 &boundsMax, const VKPT::Triangle &tri) {
    boundsMin = glm::min(boundsMin, glm::min(tri.posA, glm::min(tri.posB, tri.posC)));
    boundsMax = glm::max(boundsMax, glm::max(tri.posA, glm::max(tri.posB, tri.posC)));
}

inline float nodeCost(const glm::vec3 &boundsMin, const glm::vec3 &boundsMax, uint32_t triangleCount) {
    glm::vec3 size = boundsMax - boundsMin;
    float halfArea = size.x * (size.y + size.z) + size.y * size.z;
    return halfArea * triangleCount;
}

void computeBounds(const Vulkan::SceneManager& sceneManager, VKPT::BVH &bvh);

}
