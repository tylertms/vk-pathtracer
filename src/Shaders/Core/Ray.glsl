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
    vec2 uv = -vec2(u, v);

    if (bounceNum == 0) {
        uv += randUnitCircle(state) * ENV_TEX_BLUR;
    }

    return clamp(texture(envSampler, uv).rgb, 0, 20);
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
    vec3 throughput = vec3(1.0);
    vec3 radiance = vec3(0.0);
    float currentIOR = 1.0, glassIOR = 1.5;
    vec3 glassAbsorption = vec3(0.6);
    
    for (uint bounce = 0; bounce <= maxBounces; bounce++) {
        HitPayload hit = rayHitScene(ray, stats);
        if (!hit.didHit) {
            radiance += throughput * getEnvironmentLight(ray.dir, bounce, state);
            break;
        }
        
        if (currentIOR == glassIOR)
            throughput *= exp(-glassAbsorption * hit.distance);
        
        if (hit.material.specularFactor > 0.99) {
            bool entering = dot(ray.dir, hit.normal) < 0.0;
            vec3 n = entering ? hit.normal : -hit.normal;
            float etai = currentIOR;
            float etat = entering ? glassIOR : 1.0;
            ray.origin = entering ? hit.point - hit.normal * EPSILON : hit.point + hit.normal * EPSILON;
            float eta = etai / etat;
            float cosTheta = clamp(dot(-ray.dir, n), 0.0, 1.0);
            float k = 1.0 - eta * eta * (1.0 - cosTheta * cosTheta);
            bool totalInternal = (k < 0.0);
            float R0 = pow((etai - etat) / (etai + etat), 2.0);
            float fresnel = totalInternal ? 1.0 : (R0 + (1.0 - R0) * pow(1.0 - cosTheta, 5.0));
            
            if (rand(state) < fresnel)
                ray.dir = normalize(reflect(ray.dir, n));
            else {
                ray.dir = normalize(refract(ray.dir, n, eta));
                currentIOR = entering ? glassIOR : 1.0;
            }
        } else {
            ray.origin = hit.point + hit.normal * EPSILON;
            vec3 diffuseDir = normalize(hit.normal + randDir(state));
            vec3 specularDir = reflect(ray.dir, hit.normal);
            uint specularScatter = (hit.material.specularFactor >= rand(state)) ? 1u : 0u;
            ray.dir = normalize(mix(diffuseDir, specularDir, (1.0 - hit.material.roughness) * float(specularScatter)));
        }
        
        ray.inv = 1.0 / ray.dir;
        vec3 emitted = hit.material.emissionColor * hit.material.emissionStrength;
        radiance += throughput * emitted;
        throughput *= mix(hit.material.color, hit.material.specularColor, hit.material.specularFactor);
    }
    
    return radiance;
}



#endif
