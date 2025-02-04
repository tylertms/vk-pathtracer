#ifndef CORE_OBJECTS_BVH_H
#define CORE_OBJECTS_BVH_H

#ifdef __cplusplus
/* ---------- C++ ---------- */
#include <glm/glm.hpp>
using namespace glm;

namespace VKPT {
struct alignas(16) BVH {
    vec3 boundsMin = vec3(FLT_MAX);
    uint32_t index = 0;
    vec3 boundsMax = vec3(-FLT_MAX);
    uint32_t triangleCount = 0;
};
} // namespace VKPT

/* ---------- C++ ---------- */
#else
/* ---------- GLSL ---------- */

struct BVH {
    vec3 boundsMin;
    uint index;
    vec3 boundsMax;
    uint triangleCount;
};

float rayHitAABB(Ray ray, vec3 boundsMin, vec3 boundsMax) {
    vec3 tMin = (boundsMin - ray.origin) * ray.inv;
    vec3 tMax = (boundsMax - ray.origin) * ray.inv;
    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);
    float tNear = max(max(t1.x, t1.y), t1.z);
    float tFar = min(min(t2.x, t2.y), t2.z);

    bool hit = tFar >= tNear && tFar > 0;
    float dst = hit ? tNear > 0 ? tNear : 0 : 1.#INF;
    return dst;
}

/* ---------- GLSL ---------- */
#endif
#endif
