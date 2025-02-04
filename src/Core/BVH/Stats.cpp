#include "Stats.h"

namespace BVH {

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

}
