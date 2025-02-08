#ifndef BXDF_DIFFUSE
#define BXDF_DIFFUSE

vec3 diffuseBRDF(vec3 normal, inout uint state) {
    return normalize(normal + randDir(state));
}

#endif
