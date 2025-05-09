#ifndef CORE_RAY_GLSL
#define CORE_RAY_GLSL

#include "Common.glsl"
#include "Random.glsl"
#include "../BSDF/Evaluate.glsl"

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
    float v = 0.5 - asin(rotatedDir.y) * INV_PI;
    vec2 uv = vec2(-u, v);

    if (bounceNum == 0) {
        uv += randUnitCircle(state) * ENV_TEX_BLUR;
    }

    vec4 envTexColor = texture(sampler2D(textures[0], texSampler), uv);
    return clamp(envTexColor.rgb, 0, 20);
}

Ray generateRay(vec2 uv, inout uint state) {
    vec3 forward = normalize(scene.camera.lookAt - scene.camera.lookFrom);
    vec3 worldUp = vec3(0, 1, 0);

    vec3 right = normalize(cross(forward, worldUp));
    vec3 up = normalize(cross(right, forward));

    float tanHalfFov = tan(radians(scene.camera.vfov) * 0.5);
    vec2 recipRes = vec2(1.0) / vec2(scene.camera.windowSize);

    float halfHeight = tanHalfFov * scene.camera.focalDistance;
    float halfWidth = halfHeight * (scene.camera.windowSize.x * recipRes.y);

    vec3 focalCenter = scene.camera.lookFrom + forward * scene.camera.focalDistance;
    vec2 jitter = randUnitCircle(state) * recipRes.x;
    vec3 targetPoint = focalCenter
        + right * (uv.x * halfWidth + jitter.x * scene.camera.diverge)
        - up * (uv.y * halfHeight + jitter.y * scene.camera.diverge);

    vec3 rayOrigin = scene.camera.lookFrom;
    rayOrigin += right * (jitter.x * scene.camera.defocus);
    rayOrigin += up * (jitter.y * scene.camera.defocus);

    Ray ray;
    ray.origin = rayOrigin;
    ray.dir = normalize(targetPoint - rayOrigin);
    ray.inv = 1.0 / ray.dir;

    return ray;
}

vec3 traceRay(Ray ray, uint maxBounces, inout uint state, inout uint stats[2]) {
    vec3 throughput = vec3(1.0);
    vec3 radiance = vec3(0.0);

    for (uint bounce = 0; bounce <= maxBounces; bounce++) {
        HitPayload hit = rayHitScene(ray, stats);
        if (!hit.didHit) {
            radiance += throughput * getEnvironmentLight(ray.dir, bounce, state);
            break;
        }

        //return vec3(hit.uv, 0.0);

#ifdef DEBUG_NORMAL
        if (hit.material.normalTextureIndex > 0) {
            vec3 tangentSpaceNormal = texture(sampler2D(textures[hit.material.normalTextureIndex], texSampler), hit.uv0).rgb;

            vec3 T = normalize(hit.tangent);
            vec3 N = normalize(hit.normal);
            vec3 B = normalize(cross(N, T));

            hit.normal = normalize(mat3(T, B, N) * tangentSpaceNormal);
        }

        return hit.normal * 0.5 + 0.5;
#endif

#ifdef DEBUG_NO_PATH
        return hit.material.baseColor;
#endif

        evaluateBSDF(hit, ray, radiance, throughput, state);
    }

    return radiance;
}



#endif
