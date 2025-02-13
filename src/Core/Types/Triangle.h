#ifndef CORE_OBJECTS_TRIANGLE_H
#define CORE_OBJECTS_TRIANGLE_H

#ifdef __cplusplus
/* ---------- C++ ---------- */
#include <glm/glm.hpp>
using namespace glm;

namespace VKPT {
    struct alignas(16) Triangle {
        vec4 pos[3];
        vec4 norm[3];
        vec4 tan[3];
        vec2 uv0[3];
    };
}

/* ---------- C++ ---------- */
#else
/* ---------- GLSL ---------- */

struct Triangle {
    vec3 pos[3];
    vec3 norm[3];
    vec3 tan[3];
    vec2 uv0[3];
};

HitPayload rayHitTriangle(Ray ray, Triangle tri) {
    vec3 eAB = tri.pos[1] - tri.pos[0];
    vec3 eAC = tri.pos[2] - tri.pos[0];
    vec3 nG  = cross(eAB, eAC);
    
    float d  = -dot(ray.dir, nG);
    float ad = abs(d);
    float passDet = step(DST_EPSILON, ad);
    float invD = 1.0f / (d + 1e-30f);

    vec3 ao  = ray.origin - tri.pos[0];
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

    vec3 nI = normalize(tri.norm[0] * w + tri.norm[1] * u + tri.norm[2] * v);
    vec2 hitUV0 = tri.uv0[0] * w + tri.uv0[1] * u + tri.uv0[2] * v;
    vec3 nT = normalize(tri.tan[0] * w + tri.tan[1] * u + tri.tan[2] * v);

    HitPayload p;
    p.didHit = (passAll != 0.0f);
    p.distance = mix(0.0f, t, passAll);

    vec3 hitP = ray.origin + ray.dir * t + nI * 1e6;

    p.point = mix(vec3(0.0f), hitP, passAll);
    p.normal = mix(vec3(0.0f), nI, passAll);
    p.tangent = mix(vec3(0.0f), nT, passAll);
    p.uv0 = mix(vec2(0.0f), hitUV0, passAll);

    return p;
}

/* ---------- GLSL ---------- */
#endif
#endif
