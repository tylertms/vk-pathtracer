#ifndef CORE_RAY_GLSL
#define CORE_RAY_GLSL

#include "Common.glsl"
#include "Random.glsl"

//#define DEBUG_NORMAL

Ray generateRay(vec2 uv, float aspectRatio) {
    uv.x *= aspectRatio;

    Ray ray;
    ray.origin = vec3(0, 0, -6);
    ray.dir = normalize(vec3(uv, -2) - ray.origin);
    ray.inv = 1.0 / ray.dir;

    return ray;
}

vec3 traceRay(Ray ray, inout uint state) {
    vec3 rayColor = vec3(1);
    vec3 incomingLight = vec3(0);

    for (uint i = 0; i <= MAX_BOUNCES; i++) {
        HitPayload hit = rayHitScene(ray);
        if (!hit.didHit)
            break;

#ifdef DEBUG_NORMAL
        return hit.normal * vec3(1, 1, -1);
#endif

        ray.origin = hit.point;
        ray.dir = normalize(hit.normal + randDir(state));
        ray.inv = 1.0 / ray.dir;

        vec3 emittedLight = hit.material.emissionColor * hit.material.emissionStrength;
        incomingLight += emittedLight * rayColor;
        rayColor *= hit.material.color;
    }

    return incomingLight;
}

#endif
