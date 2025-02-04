#include "Builder.h"

#include "../Constants.h"

#include <algorithm>
#include <cfloat>
#include <chrono>
#include <functional>

namespace BVH {

inline void growBounds(glm::vec3 &boundsMin, glm::vec3 &boundsMax, const VKPT::Triangle &tri) {
    boundsMin = glm::min(boundsMin, glm::min(tri.posA, glm::min(tri.posB, tri.posC)));
    boundsMax = glm::max(boundsMax, glm::max(tri.posA, glm::max(tri.posB, tri.posC)));
}

void computeBounds(const Vulkan::SceneManager& sceneManager, VKPT::BVH &bvh) {
    for (uint32_t i = bvh.index; i < bvh.index + bvh.triangleCount; i++) {
        const VKPT::Triangle &tri = sceneManager.sceneStorage->triangles[i];
        growBounds(bvh.boundsMin, bvh.boundsMax, tri);
    }
}

inline float nodeCost(const glm::vec3 &boundsMin, const glm::vec3 &boundsMax, uint32_t triangleCount) {
    glm::vec3 size = boundsMax - boundsMin;
    float halfArea = size.x * (size.y + size.z) + size.y * size.z;
    return halfArea * triangleCount;
}

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

void splitBVH(Vulkan::SceneManager &sceneManager, uint32_t bvhIndex, uint8_t depth = 0) {
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

void collectAndPrintStats(Vulkan::SceneManager &sceneManager, uint32_t rootIndex, float buildTime) {
    const auto& bvhs = sceneManager.sceneStorage->bvhs;

    struct TraversalStats {
        int totalNodes = 0;
        int leafNodes = 0;
        int maxTriangles = 0;
        int minTriangles = INT_MAX;
        int totalTriangles = 0;
        int maxDepth = 0;
        int minLeafDepth = INT_MAX;
        int maxLeafDepth = 0;
        int totalLeafDepth = 0;
    } stats;

    std::function<void(uint32_t, int)> traverse = [&](uint32_t index, int depth) {
        const auto& node = bvhs[index];
        stats.totalNodes++;
        stats.maxDepth = std::max(stats.maxDepth, depth);

        if (node.triangleCount > 0) {
            stats.leafNodes++;
            stats.totalTriangles += node.triangleCount;
            stats.maxTriangles = std::max(stats.maxTriangles, static_cast<int>(node.triangleCount));
            stats.minTriangles = std::min(stats.minTriangles, static_cast<int>(node.triangleCount));
            stats.minLeafDepth = std::min(stats.minLeafDepth, depth);
            stats.maxLeafDepth = std::max(stats.maxLeafDepth, depth);
            stats.totalLeafDepth += depth;
        } else {
            traverse(node.index, depth + 1);
            traverse(node.index + 1, depth + 1);
        }
    };

    traverse(rootIndex, 0);

    const float avgDepth = stats.leafNodes ? static_cast<float>(stats.totalLeafDepth) / stats.leafNodes : 0;
    const float avgTriangles = stats.leafNodes ? static_cast<float>(stats.totalTriangles) / stats.leafNodes : 0;
    const size_t memoryUsage = stats.totalNodes * sizeof(VKPT::BVH);

    printf("\n====== BVH Statistics ======\n");
    printf("Build Time:          %.2f ms\n", buildTime);
    printf("Total Triangles:     %d\n", stats.totalTriangles);
    printf("Total Nodes:         %d\n", stats.totalNodes);
    printf("Leaf Nodes:          %d\n", stats.leafNodes);
    printf("Triangle Count:      Min: %d, Max: %d, Avg: %.1f\n",
           stats.minTriangles, stats.maxTriangles, avgTriangles);
    printf("Tree Depth:          %d\n", stats.maxDepth);
    printf("Leaf Depth:          Min: %d, Max: %d, Avg: %.1f\n",
           stats.minLeafDepth, stats.maxLeafDepth, avgDepth);
    printf("Memory Usage:        %.2f KB\n", memoryUsage / 1024.0f);
    printf("============================\n\n");
}

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

} // namespace BVH
