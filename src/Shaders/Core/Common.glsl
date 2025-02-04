#ifndef CORE_COMMON_GLSL
#define CORE_COMMON_GLSL

#include "../../Core/Types/Material.h"

struct Ray {
    vec3 origin;
    vec3 dir;
    vec3 inv;
};

struct HitPayload {
    bool didHit;
    vec3 point;
    vec3 normal;
    float distance;
    Material material;
};

vec3 toneMapACES(vec3 x) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

#endif
