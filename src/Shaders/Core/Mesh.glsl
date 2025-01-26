#ifndef CORE_MESH_GLSL
#define CORE_MESH_GLSL

#include "Common.glsl"

struct Mesh {
    vec3 translation;
    uint triangleCount;
    vec3 scale;
    uint startIndex;
    vec3 rotation;
    vec3 boundsMin;
    vec3 boundsMax;
    Material material;
};

#endif