#ifndef CORE_RAY_GLSL
#define CORE_RAY_GLSL

#include "Common.glsl"
#include "Random.glsl"

#define DEBUG_NORMAL

Ray generateRay(vec2 uv, Camera camera, inout uint state) {
    uv.x *= float(camera.windowSize.x) / camera.windowSize.y;
    vec2 planeJitter = randUnitCircle(state) * camera.diverge / camera.windowSize.x;
    vec2 originJitter = randUnitCircle(state) * camera.defocus / camera.windowSize.x;

    Ray ray;
    ray.origin = vec3(0, 0, -6);
    ray.origin += vec3(originJitter.x, originJitter.y, 0);

    vec3 targetPoint = vec3((uv + planeJitter) * camera.focalDistance / 4.f, ray.origin.z + camera.focalDistance);
    ray.dir = normalize(targetPoint - ray.origin);
    ray.inv = 1.0 / ray.dir;

    return ray;
}

vec3 traceRay(Ray ray, uint maxBounces, inout uint state) {
    vec3 rayColor = vec3(1);
    vec3 incomingLight = vec3(0);

    for (uint i = 0; i <= maxBounces; i++) {
        HitPayload hit = rayHitScene(ray);
        if (!hit.didHit)
            break;

#ifdef DEBUG_NORMAL
        return hit.normal * vec3(1, 1, -1);
#endif

        ray.origin = hit.point;

        vec3 diffuseDir = normalize(hit.normal + randDir(state));
        vec3 specularDir = reflect(ray.dir, hit.normal);
        uint specularScatter = hit.material.specularFactor >= rand(state) ? 1 : 0;

        ray.dir = mix(diffuseDir, specularDir, (1 - hit.material.roughness) * specularScatter);
        ray.inv = 1.0 / ray.dir;

        vec3 emittedLight = hit.material.emissionColor * hit.material.emissionStrength;
        incomingLight += emittedLight * rayColor;
        rayColor *= mix(hit.material.color, hit.material.specularColor, specularScatter);
    }

    return incomingLight;
}

#endif
