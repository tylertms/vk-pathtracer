#ifndef CORE_OBJECTS_MESH_H
#define CORE_OBJECTS_MESH_H

#include "AABB.h"
#include "Material.h"

#ifdef __cplusplus
/* ---------- C++ ---------- */
#include <glm/glm.hpp>
using namespace glm;

namespace VKPT {
struct alignas(16) Mesh {
    mat4 worldLocalTransform;
    mat4 localWorldTransform;
    uint32_t triangleCount;
    uint32_t startIndex;
    AABB bounds;
    Material material;
};
}

namespace YAML {
template <>
struct convert<VKPT::Mesh> {
    static Node encode(const VKPT::Mesh &rhs) {
        Node node;
        node["Material"] = rhs.material;
        return node;
    }

    static bool decode(const Node &node, VKPT::Mesh &rhs) {
        if (!node.IsMap() || !node["File"]) {
            return false;
        }

        if (node["Material"]) {
            rhs.material = node["Material"].as<VKPT::Material>();
        }

        return true;
    }
};
} // namespace YAML

/* ---------- C++ ---------- */
#else
/* ---------- GLSL ---------- */

struct Mesh {
    mat4 worldLocalTransform;
    mat4 localWorldTransform;
    uint triangleCount;
    uint startIndex;
    AABB bounds;
    Material material;
};

/* ---------- GLSL ---------- */
#endif
#endif
