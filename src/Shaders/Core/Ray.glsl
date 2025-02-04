#ifndef CORE_RAY_GLSL
#define CORE_RAY_GLSL

#include "Common.glsl"
#include "Random.glsl"

// #define DEBUG_NORMAL

vec3 getEnvironmentLight(vec3 dir, uint bounceNum, inout uint state) {
    float angle = radians(scene.camera.envRotation);
    float cosA = cos(angle);
    float sinA = sin(angle);

    vec3 rotatedDir = vec3(
        cosA * dir.x - sinA * dir.z,
        dir.y,
        sinA * dir.x + cosA * dir.z
    );

    float u = 0.5 + atan(rotatedDir.z, rotatedDir.x) * INV_2PI;
    float v = -0.5 + asin(rotatedDir.y) * INV_PI;
    vec2 uv = vec2(u, v);

    if (bounceNum == 0) {
        uv += randUnitCircle(state) * ENV_TEX_BLUR;
    }

    return clamp(texture(envSampler, uv).rgb, 0, 10);
}

Ray generateRay(vec2 uv, inout uint state) {
    uv.x *= float(scene.camera.windowSize.x) / scene.camera.windowSize.y;
    vec2 planeJitter = randUnitCircle(state) * scene.camera.diverge / scene.camera.windowSize.x;
    vec2 originJitter = randUnitCircle(state) * scene.camera.defocus / scene.camera.windowSize.x;

    Ray ray;
    ray.origin = vec3(0, 0, -6);
    ray.origin += vec3(originJitter.x, originJitter.y, 0);

    vec3 targetPoint = vec3((uv + planeJitter) * scene.camera.focalDistance / 4., ray.origin.z + scene.camera.focalDistance);
    ray.dir = normalize(targetPoint - ray.origin);
    ray.inv = 1.0 / ray.dir;

    return ray;
}

vec3 traceRay(Ray ray, uint maxBounces, inout uint state, inout uint stats[2]) {
    vec3 rayColor = vec3(1);
    vec3 incomingLight = vec3(0);

    for (uint i = 0; i <= maxBounces; i++) {
        HitPayload hit = rayHitScene(ray, stats);
        if (!hit.didHit) {
            incomingLight += getEnvironmentLight(ray.dir, i, state) * rayColor;
            break;
        }

#ifdef DEBUG_NORMAL
        return hit.normal * vec3(1, 1, -1);// * 0.5f + vec3(0.5);
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
