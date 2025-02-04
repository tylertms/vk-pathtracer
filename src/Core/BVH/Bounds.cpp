#include "Bounds.h"

namespace BVH {

void computeBounds(const Vulkan::SceneManager& sceneManager, VKPT::BVH &bvh) {
    for (uint32_t i = bvh.index; i < bvh.index + bvh.triangleCount; i++) {
        const VKPT::Triangle &tri = sceneManager.sceneStorage->triangles[i];
        growBounds(bvh.boundsMin, bvh.boundsMax, tri);
    }
}

}
