#ifndef BXDF_GLASS
#define BXDF_GLASS

#include "../../Core/Constants.h"

// Complex IOR
const vec2 GLASS_IOR = vec2(1.5, 0.4);

// TODO: Implement wavlength-dependent refraction
// const float WAVELENGTH = 550e-9;
// const float ABSORPTION = 4 * PI * GLASS_IOR.y / WAVELENGTH;

// return true if refraction is impossible
bool totalInternalReflection(vec3 incident, vec3 normal, float eta) {
    float cosTheta = dot(-incident, normal);
    float k = 1.0 - eta * eta * (1.0 - cosTheta * cosTheta);
    return (k < 0.0);
}

vec3 glassBSDF(inout vec3 normal, in vec3 incident, float segmentLength, inout vec3 throughput, inout uint state) {
    float cosTheta_i = dot(normal, incident);
    bool entering = cosTheta_i < 0.0;
    normal = entering ? normal : -normal;

    float realIOR = GLASS_IOR.x;
    float eta = entering ? 1.0 / realIOR : realIOR;
    float cosTheta = clamp(dot(-incident, normal), 0.0, 1.0);
    float R0 = pow((1.0 - realIOR) / (1.0 + realIOR), 2.0);
    float fresnel = R0 + (1.0 - R0) * pow(1.0 - cosTheta, 5.0);
    bool reflectSample = rand(state) < fresnel;

    vec3 absorption = vec3(entering ? 1.0 : exp(-GLASS_IOR.y * segmentLength));
    throughput *= absorption;

    if (reflectSample || totalInternalReflection(incident, normal, eta)) {
        return reflect(incident, normal);
    } else {
        return refract(incident, normal, eta);
    }
}


#endif
