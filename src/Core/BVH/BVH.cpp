#include "BVH.h"

namespace BVH {

void createBVH(const Vulkan::SceneManager &sceneManager) {
    for (uint32_t i = 0; i < sceneManager.sceneData.numMeshes; i++) {
        VKPT::Mesh mesh = sceneManager.sceneData.meshes[i];
        printf("Creating BVHs for mesh %d/%d\n", i + 1, sceneManager.sceneData.numMeshes);
        printf("Triangles - start: %d, count: %d\n", mesh.startIndex, mesh.triangleCount);
    }
}

}
