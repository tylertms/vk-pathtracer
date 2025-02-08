#ifndef BXDF_SPECULAR
#define BXDF_SPECULAR

vec3 specularBRDF(vec3 normal, vec3 incident) {
    return normalize(reflect(incident, normal));
}

#endif
