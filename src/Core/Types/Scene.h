#ifndef CORE_OBJECTS_SCENEOBJECT_H
#define CORE_OBJECTS_SCENEOBJECT_H

#include "../Constants.h"

#include "Camera.h"
#include "Mesh.h"
#include "Triangle.h"
#include "Sphere.h"

#ifdef __cplusplus
/* ---------- C++ ---------- */

namespace VKPT {
struct alignas(16) SceneData {
    Camera camera;
    uint32_t framesRendered = 0;

    uint32_t numSpheres = 0;
    uint32_t numMeshes = 0;
    uint32_t numTriangles = 0;
};

struct alignas(16) SceneStorage {
    Sphere spheres[MAX_SPHERES];
    Mesh meshes[MAX_MESHES];
    Triangle triangles[MAX_TRIANGLES];
};
}

/* ---------- C++ ---------- */
#endif
#endif
