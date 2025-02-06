#ifndef CORE_RAY_GLSL
#define CORE_RAY_GLSL

#include "Common.glsl"
#include "Random.glsl"

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
    vec2 uv = vec2(u, v);

    if (bounceNum == 0) {
        uv += randUnitCircle(state) * ENV_TEX_BLUR;
    }

    return clamp(texture(envSampler, uv).rgb, 0, 20);
}

Ray generateRay(vec2 uv, inout uint state) {
    vec3 forward = normalize(scene.camera.lookAt - scene.camera.lookFrom);
    vec3 worldUp = vec3(0, 1, 0);
    
    vec3 right = normalize(cross(forward, mix(worldUp, vec3(0,0,1), 
        float(abs(dot(forward, worldUp)) > 0.99))));
    vec3 up = normalize(cross(right, forward));

    float tanHalfFov = tan(radians(scene.camera.vfov) * 0.5);
    vec2 recipRes = vec2(1.0) / vec2(scene.camera.windowSize);
    
    float halfHeight = tanHalfFov * scene.camera.focalDistance;
    float halfWidth = halfHeight * (scene.camera.windowSize.x * recipRes.y);

    vec3 focalCenter = scene.camera.lookFrom + forward * scene.camera.focalDistance;
    vec2 jitter = randUnitCircle(state) * recipRes.x;
    vec3 targetPoint = focalCenter 
        - right * (uv.x * halfWidth + jitter.x * scene.camera.diverge)
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

#ifdef DEBUG_NORMAL
            return hit.normal * 0.5 + 0.5;
#endif
        
        vec3 diffuseDir = normalize(hit.normal + randDir(state));
        vec3 specularDir = normalize(reflect(ray.dir, hit.normal));
        uint specularScatter = (hit.material.specularFactor >= rand(state)) ? 1u : 0u;

        ray.origin = hit.point + hit.normal * RAY_ORIGIN_EPSILON;
        ray.dir = normalize(mix(diffuseDir, specularDir, (1.0 - hit.material.roughness) * float(specularScatter)));
        ray.inv = 1.0 / ray.dir;

        vec3 emitted = hit.material.emissionColor * hit.material.emissionStrength;
        radiance += throughput * emitted;
        throughput *= mix(hit.material.color, hit.material.specularColor, hit.material.specularFactor);
    }
    
    return radiance;
}



#endif
