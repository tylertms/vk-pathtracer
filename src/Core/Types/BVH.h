#ifndef CORE_OBJECTS_BVH_H
#define CORE_OBJECTS_BVH_H

#include "AABB.h"

#ifdef __cplusplus
/* ---------- C++ ---------- */
#include <glm/glm.hpp>
using namespace glm;

namespace VKPT {
struct alignas(16) BVH {
    AABB bounds;
    uint32_t meshIndex;
    uint32_t leftChild;
    uint32_t triangleIndex;
    uint32_t triangleCount;
};
}

/* ---------- C++ ---------- */
#else
/* ---------- GLSL ---------- */

struct BVH {
    AABB bounds;
    uint meshIndex;
    uint leftChild;
    uint triangleIndex;
    uint triangleCount;
};

/* ---------- GLSL ---------- */
#endif
#endif
