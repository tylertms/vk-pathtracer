#ifndef BSDF_SPECULAR
#define BSDF_SPECULAR

#include "Diffuse.glsl"

vec3 specularBRDF(vec3 normal, vec3 incident, inout uint state) {
    return normalize(reflect(incident, normal));
}

#endif
