#ifndef CORE_OBJECTS_TRIANGLE_H
#define CORE_OBJECTS_TRIANGLE_H

#ifdef __cplusplus
/* ---------- C++ ---------- */
#include <glm/glm.hpp>
using namespace glm;

namespace VKPT {
struct alignas(16) Triangle {
    alignas(16) vec3 posA;
    alignas(16) vec3 posB;
    alignas(16) vec3 posC;
    alignas(16) vec3 normA;
    alignas(16) vec3 normB;
    alignas(16) vec3 normC;
};
}

/* ---------- C++ ---------- */
#else
/* ---------- GLSL ---------- */

struct Triangle {
    vec3 posA;
    vec3 posB;
    vec3 posC;
    vec3 normA;
    vec3 normB;
    vec3 normC;
};

HitPayload rayHitTriangle(Ray ray, Triangle tri) {
    vec3 edgeAB = tri.posB - tri.posA;
    vec3 edgeAC = tri.posC - tri.posA;
    vec3 normalVector = cross(edgeAB, edgeAC);
    vec3 ao = ray.origin - tri.posA;
    vec3 dao = cross(ao, ray.dir);

    float determinant = -dot(ray.dir, normalVector);
    float invDet = 1 / determinant;

    float dst = dot(ao, normalVector) * invDet;
    float u = dot(edgeAC, dao) * invDet;
    float v = -dot(edgeAB, dao) * invDet;
    float w = 1 - u - v;

    HitPayload hit;
    hit.didHit = determinant >= EPSILON && dst >= 0 && u >= 0 && v >= 0 && w >= 0;
    hit.point = ray.origin + ray.dir * dst;
    hit.normal = normalize(tri.normA * w + tri.normB * u + tri.normC * v);
    hit.distance = dst;
    return hit;
}

/* ---------- GLSL ---------- */
#endif
#endif
