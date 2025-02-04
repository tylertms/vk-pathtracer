#include "Builder.h"

#include "Stats.h"
#include "Bounds.h"
#include "Split.h"
#include "../Constants.h"

#include <algorithm>

namespace BVH {

void createBVH(VKPT::Mesh &mesh, VKPT::BVH &rootBVH, Vulkan::SceneManager &sceneManager) {
    computeBounds(sceneManager, rootBVH);
    uint32_t rootIndex = sceneManager.sceneData.numBVHs++;
    sceneManager.sceneStorage->bvhs[rootIndex] = rootBVH;

    auto start = std::chrono::high_resolution_clock::now();
    splitBVH(sceneManager, rootIndex);
    auto end = std::chrono::high_resolution_clock::now();
    float buildTime = std::chrono::duration<float, std::milli>(end - start).count();

    collectAndPrintStats(sceneManager, rootIndex, buildTime);
}

void splitBVH(Vulkan::SceneManager &sceneManager, uint32_t bvhIndex, uint8_t depth) {
    VKPT::BVH &bvh = sceneManager.sceneStorage->bvhs[bvhIndex];
    if (bvh.triangleCount <= BVH_TRIANGLE_MIN || depth > BVH_MAX_DEPTH) return;

    SplitData split = chooseSplit(sceneManager, bvh);
    if (split.cost >= nodeCost(bvh.boundsMin, bvh.boundsMax, bvh.triangleCount)) return;

    uint32_t start = bvh.index;
    uint32_t end = start + bvh.triangleCount;
    uint32_t leftCount = 0;

    for (uint32_t i = start; i < end; i++) {
        VKPT::Triangle& tri = sceneManager.sceneStorage->triangles[i];
        vec3 centroid = (tri.posA + tri.posB + tri.posC) / 3.0f;
        if (centroid[split.axis] < split.pos) {
            std::swap(tri, sceneManager.sceneStorage->triangles[start + leftCount]);
            leftCount++;
        }
    }

    if (leftCount == 0 || leftCount == bvh.triangleCount) return;

    uint32_t leftIndex = sceneManager.sceneData.numBVHs++;
    uint32_t rightIndex = sceneManager.sceneData.numBVHs++;
    VKPT::BVH& left = sceneManager.sceneStorage->bvhs[leftIndex];
    VKPT::BVH& right = sceneManager.sceneStorage->bvhs[rightIndex];

    left.index = start;
    left.triangleCount = leftCount;
    right.index = start + leftCount;
    right.triangleCount = bvh.triangleCount - leftCount;

    computeBounds(sceneManager, left);
    computeBounds(sceneManager, right);

    bvh.index = leftIndex;
    bvh.triangleCount = 0;

    splitBVH(sceneManager, leftIndex, depth + 1);
    splitBVH(sceneManager, rightIndex, depth + 1);
}

} // namespace BVH
