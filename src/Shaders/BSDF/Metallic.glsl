#ifndef BSDF_METALLIC
#define BSDF_METALLIC

vec3 metallicBSDF(vec3 normal, vec3 incident, Material material, out vec3 fresnel, inout uint state) {
    vec3 microNormal = sampleGGX(normal, material.roughness, state);
    vec3 reflectionDir = reflect(incident, microNormal);
    float cosTheta = clamp(dot(-incident, microNormal), 0.0, 1.0);
    fresnel = material.baseColor + (vec3(1.0) - material.baseColor) * pow(1.0 - cosTheta, 5.0);

    return reflectionDir;
}

#endif
