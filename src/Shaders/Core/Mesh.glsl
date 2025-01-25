#ifndef CORE_MESH_GLSL
#define CORE_MESH_GLSL

#include "Common.glsl"

struct Mesh {
    uint triangleCount;
    uint startIndex;
    Material material;
};

#endif