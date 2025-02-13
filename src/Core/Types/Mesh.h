#ifndef CORE_OBJECTS_MESH_H
#define CORE_OBJECTS_MESH_H

#include "Material.h"

#ifdef __cplusplus
/* ---------- C++ ---------- */
#include <glm/glm.hpp>
using namespace glm;

namespace VKPT {
struct alignas(16) Mesh {
    mat4 worldLocalTransform;
    mat4 localWorldTransform;
    uint32_t materialIndex = 0;
    uint32_t rootBVHNode = 0;
};
} // namespace VKPT

namespace YAML {
template <>
struct convert<VKPT::Mesh> {
    static Node encode(const VKPT::Mesh &rhs) {
        Node node;
        node["MaterialIndex"] = rhs.materialIndex;
        return node;
    }

    static bool decode(const Node &node, VKPT::Mesh &rhs) {
        if (!node.IsMap() || !node["File"]) {
            return false;
        }

        if (node["MaterialIndex"]) {
            rhs.materialIndex = node["MaterialIndex"].as<uint32_t>();
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
    uint materialIndex;
    uint rootBVHNode;
};

/* ---------- GLSL ---------- */
#endif
#endif
