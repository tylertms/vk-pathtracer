#include "Split.h"

#include "Bounds.h"

namespace BVH {

float evaluateSplit(const Vulkan::SceneManager &sceneManager, const VKPT::BVH &bvh, int splitAxis, float splitPos) {
    glm::vec3 boundsAmin = glm::vec3(FLT_MAX);
    glm::vec3 boundsBmin = glm::vec3(FLT_MAX);

    glm::vec3 boundsAmax = glm::vec3(-FLT_MAX);
    glm::vec3 boundsBmax = glm::vec3(-FLT_MAX);

    uint32_t numInA = 0;
    uint32_t numInB = 0;

    for (uint32_t i = bvh.index; i < bvh.index + bvh.triangleCount; i++) {
        VKPT::Triangle tri = sceneManager.sceneStorage->triangles[i];
        vec3 centroid = (tri.posA + tri.posB + tri.posC) / 3.0f;

        if (centroid[splitAxis] < splitPos) {
            growBounds(boundsAmin, boundsAmax, tri);
            numInA++;
        } else {
            growBounds(boundsBmin, boundsBmax, tri);
            numInB++;
        }
    }

    return nodeCost(boundsAmin, boundsAmax, numInA) + nodeCost(boundsBmin, boundsBmax, numInB);
}

SplitData chooseSplit(const Vulkan::SceneManager &sceneManager, const VKPT::BVH &bvh) {
    SplitData best;

    for (uint32_t axis = 0; axis < 3; axis++) {
        float boundsStart = bvh.boundsMin[axis];
        float boundsEnd = bvh.boundsMax[axis];

        for (int i = 0; i < BVH_AXIS_TESTS; i++) {
            float splitT = (i + 1) / (BVH_AXIS_TESTS + 1.f);
            float pos = boundsStart + (boundsEnd - boundsStart) * splitT;
            float cost = evaluateSplit(sceneManager, bvh, axis, pos);

            if (cost < best.cost) {
                best.cost = cost;
                best.pos = pos;
                best.axis = axis;
            }
        }
    }

    return best;
}

} // namespace BVH
