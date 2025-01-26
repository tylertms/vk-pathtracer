#ifndef CORE_OBJECTS_AABB_H
#define CORE_OBJECTS_AABB_H

#include "Material.h"

#ifdef __cplusplus
/* ---------- C++ ---------- */
#include <glm/glm.hpp>
using namespace glm;

namespace VKPT {
struct alignas(16) AABB {
    alignas(16) vec3 min = { FLT_MAX, FLT_MAX, FLT_MAX };
    alignas(16) vec3 max = -min;
};
}

/* ---------- C++ ---------- */
#else
/* ---------- GLSL ---------- */

struct AABB {
    vec3 min;
    vec3 max;
};

// https://tavianator.com/2011/ray_box.html
float rayHitAABB(Ray ray, AABB box) {
    vec3 tMin = (box.min - ray.origin) * ray.inv;
    vec3 tMax = (box.max - ray.origin) * ray.inv;
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
