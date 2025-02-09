#ifndef BSDF_GLASS
#define BSDF_GLASS

#include "../../Core/Constants.h"
#include "../Utility/GGX.glsl"

// TODO: Implement wavlength-dependent refraction
// const float WAVELENGTH = 550e-9;
// const float ABSORPTION = 4 * PI * GLASS_IOR.y / WAVELENGTH;

// return true if refraction is impossible
bool totalInternalReflection(vec3 incident, vec3 normal, float eta) {
    float cosTheta = dot(-incident, normal);
    float k = 1.0 - eta * eta * (1.0 - cosTheta * cosTheta);
    return (k < 0.0);
}

vec3 glassBSDF(HitPayload hit, inout vec3 normal, vec3 incident, inout vec3 throughput, inout uint state) {
    float cosTheta_i = dot(normal, incident);
    bool entering = cosTheta_i < 0.0;
    normal = entering ? normal : -normal;

    float realIOR = hit.material.IOR.x;
    float eta = entering ? 1.0 / realIOR : realIOR;

    float cosTheta = clamp(dot(-incident, normal), 0.0, 1.0);
    float R0 = pow((1.0 - realIOR) / (1.0 + realIOR), 2.0);
    float fresnel = R0 + (1.0 - R0) * pow(1.0 - cosTheta, 5.0);

    vec3 microNormal = sampleGGX(normal, hit.material.roughness, state);

    vec3 absorption = vec3(entering ? 1.0 : exp(-hit.material.IOR.y * hit.distance));
    throughput *= absorption;

    bool reflectSample = (rand(state) < fresnel);

    if (reflectSample || totalInternalReflection(incident, microNormal, eta)) {
        return reflect(incident, microNormal);
    } else {
        normal = -normal;
        return refract(incident, microNormal, eta);
    }
}

#endif
