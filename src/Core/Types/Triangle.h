#ifndef CORE_OBJECTS_TRIANGLE_H
#define CORE_OBJECTS_TRIANGLE_H

#ifdef __cplusplus
/* ---------- C++ ---------- */
#include <glm/glm.hpp>
using namespace glm;

namespace VKPT
{
    struct alignas(16) Triangle
    {
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

struct Triangle
{
    vec3 posA;
    vec3 posB;
    vec3 posC;
    vec3 normA;
    vec3 normB;
    vec3 normC;
};

HitPayload rayHitTriangle(Ray ray, Triangle tri)
{
    HitPayload hit;
    hit.didHit = false;

    vec3 edge1 = tri.posB - tri.posA;
    vec3 edge2 = tri.posC - tri.posA;
    vec3 h = cross(ray.dir, edge2);
    float a = dot(edge1, h);

    if (abs(a) < EPSILON) return hit;

    float f = 1.0 / a;
    vec3 s = ray.origin - tri.posA;
    float u = f * dot(s, h);
    if (u < 0.0 || u > 1.0) return hit;

    vec3 q = cross(s, edge1);
    float v = f * dot(ray.dir, q);
    if (v < 0.0 || (u + v) > 1.0) return hit;

    float t = f * dot(edge2, q);
    if (t < 0.0) return hit;

    hit.didHit = true;
    hit.distance = t;
    hit.point = ray.origin + ray.dir * t;

    float w = 1.0 - u - v;

    vec3 interpNormal = normalize(tri.normA * w + tri.normB * u + tri.normC * v);
    vec3 faceNormal = normalize(cross(edge1, edge2));

    if (dot(interpNormal, faceNormal) < 0.0)
        interpNormal = -interpNormal;

    hit.normal = interpNormal;

    return hit;
}

/* ---------- GLSL ---------- */
#endif
#endif
