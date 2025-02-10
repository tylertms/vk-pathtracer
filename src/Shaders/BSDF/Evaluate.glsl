#ifndef BSDF_EVALUATE
#define BSDF_EVALUATE

#include "Diffuse.glsl"
#include "Glass.glsl"
#include "Metallic.glsl"
#include "Specular.glsl"

void evaluateBSDF(HitPayload hit, inout Ray ray, inout vec3 radiance, inout vec3 throughput, inout uint state) {
    /* ----- BRDF ----- */
    vec3 diffuseDir = diffuseBRDF(hit.normal, state);
    vec3 specularDir = specularBRDF(hit.normal, ray.dir, state);
    uint specular = (hit.material.specular / 12.5) > rand(state) ? 1 : 0;

    vec3 reflectDir = mix(diffuseDir, specularDir, (1.0 - hit.material.roughness) * specular);
    vec3 reflectNormal = hit.normal;
    /* ---------------- */

    /* ----- BTDF ----- */
    vec3 transmitNormal = hit.normal;
    vec3 glassDir = glassBSDF(hit, transmitNormal, ray.dir, throughput, state);
    vec3 transmitDir = glassDir;
    /* ---------------- */

    vec3 outDir = mix(reflectDir, transmitDir, hit.material.transmission);

    ray.origin = hit.point + mix(reflectNormal, transmitNormal, hit.material.transmission) * RAY_ORIGIN_EPSILON;
    ray.dir = outDir;
    ray.inv = 1.0 / outDir;

    vec3 emitted = hit.material.emissionColor * hit.material.emissionStrength;
    radiance += throughput * emitted;
    throughput *= mix(hit.material.baseColor, hit.material.specularTint, specular);
}

#endif
