#ifndef CORE_OBJECTS_SCENEOBJECT_H
#define CORE_OBJECTS_SCENEOBJECT_H

#include "../Constants.h"

#include "BVH.h"
#include "Camera.h"
#include "Mesh.h"
#include "Sphere.h"
#include "Triangle.h"

#ifdef __cplusplus
/* ---------- C++ ---------- */

namespace VKPT {
struct alignas(16) SceneData {
    Camera camera;
    Sphere spheres[MAX_SPHERES];
    Mesh meshes[MAX_MESHES];
    Material materials[MAX_MATERIALS];

    uint32_t numSpheres = 0;
    uint32_t numMeshes = 0;
    uint32_t numMaterials = 1;
    uint32_t numTriangles = 0;
    uint32_t numBVHs = 0;
    uint32_t framesRendered = 0;
};

struct alignas(16) SceneStorage {
    BVH bvhs[MAX_BVHS];
    Triangle triangles[MAX_TRIANGLES];
};
} // namespace VKPT

/* ---------- C++ ---------- */
#endif
#endif
