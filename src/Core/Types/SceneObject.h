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
struct alignas(16) SceneObject {
    Camera camera;
    Sphere spheres[MAX_SPHERES];
    Triangle triangles[MAX_TRIANGLES];
    Mesh meshes[MAX_MESHES];
    uint32_t numSpheres = 0;
    uint32_t numTriangles = 0;
    uint32_t numMeshes = 0;
    uint32_t framesRendered = 0;
};
}

/* ---------- C++ ---------- */
#else
/* ---------- GLSL ---------- */
/*
struct SceneObject {
    Camera camera;
    Sphere spheres[MAX_SPHERES];
    Triangle triangles[MAX_TRIANGLES];
    Mesh meshes[MAX_MESHES];
    uint numSpheres;
    uint numTriangles;
    uint numMeshes;
    uint framesRendered;
};
*/
/* ---------- GLSL ---------- */
#endif
#endif
