#include "Bounds.h"

namespace BVH {

void computeBounds(const Vulkan::SceneManager &sceneManager, VKPT::BVH &bvh) {
    for (uint32_t i = bvh.index; i < bvh.index + bvh.triangleCount; i++) {
        uint32_t triIndex = sceneManager.triIndices[i];
        growBounds(bvh.boundsMin, bvh.boundsMax, sceneManager.triMin[triIndex], sceneManager.triMax[triIndex]);
    }
}

} // namespace BVH
