#ifndef CORE_RAY_GLSL
#define CORE_RAY_GLSL

#include "Common.glsl"
#include "Random.glsl"

Ray generateRay(vec2 uv, float aspectRatio) {
    uv.x *= aspectRatio;

    Ray ray;
    ray.origin = vec3(0, 0, -6);
    ray.dir = normalize(vec3(uv, -2) - ray.origin);

    return ray;
}

vec3 traceRay(Ray ray, inout uint state) {
    vec3 rayColor = vec3(1);
    vec3 incomingLight = vec3(0);

    for (uint i = 0; i <= MAX_BOUNCES; i++) {
        HitPayload hit = rayHitScene(ray);
        if (!hit.didHit)
        break;

        ray.origin = hit.point;
        ray.dir = normalize(hit.normal + randDir(state));

        vec3 emittedLight = hit.material.emissionColor * hit.material.emissionStrength;
        incomingLight += emittedLight * rayColor;
        rayColor *= hit.material.color;
    }

    return incomingLight;
}

#endif
