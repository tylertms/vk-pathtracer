#ifndef CORE_AABB_GLSL
#define CORE_AABB_GLSL

#include "Common.glsl"

// https://tavianator.com/2011/ray_box.html
float rayHitAABB(Ray ray, Mesh mesh) {
    vec3 tMin = (mesh.boundsMin - ray.origin) * ray.inv;
    vec3 tMax = (mesh.boundsMax - ray.origin) * ray.inv;
    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);
    float tNear = max(max(t1.x, t1.y), t1.z);
    float tFar = min(min(t2.x, t2.y), t2.z);

    bool hit = tFar >= tNear && tFar > 0;
    float dst = hit ? tNear > 0 ? tNear : 0 : 1.#INF;
    return dst;
}

#endif