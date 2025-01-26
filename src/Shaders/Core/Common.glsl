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

#endif
