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
    vec3 eAB = tri.posB - tri.posA;
    vec3 eAC = tri.posC - tri.posA;
    vec3 nG  = cross(eAB, eAC);
    float d  = -dot(ray.dir, nG);
    float ad = abs(d);
    float passDet = step(DST_EPSILON, ad);
    float invD = 1.0f / (d + 1e-30f);
    vec3 ao  = ray.origin - tri.posA;
    vec3 dao = cross(ao, ray.dir);
    float t  = dot(ao, nG) * invD;
    float u  = dot(eAC, dao) * invD;
    float v  = -dot(eAB, dao) * invD;
    float w  = 1.0f - u - v;
    float passT = step(DST_EPSILON, t);
    float passU = step(0.0f, u);
    float passV = step(0.0f, v);
    float passW = step(0.0f, w);
    float passAll = passDet * passT * passU * passV * passW;

    vec3 nI = normalize(tri.normA * w + tri.normB * u + tri.normC * v);

    HitPayload p;
    p.didHit   = (passAll != 0.0f);
    p.distance = mix(0.0f, t, passAll);
    vec3 hitP  = ray.origin + ray.dir * t + nI * 1e6;
    p.point    = mix(vec3(0.0f), hitP, passAll);
    p.normal   = mix(vec3(0.0f), nI,   passAll);
    return p;
}

/* ---------- GLSL ---------- */
#endif
#endif
