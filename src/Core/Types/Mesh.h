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
    vec3 translation = { 0.0, 0.0, 0.0 };
    uint32_t triangleCount;
    vec3 scale = { 1.0, 1.0, 1.0 };
    uint32_t startIndex;
    vec3 rotation = { 0.0, 0.0, 0.0 };
    AABB bounds;
    Material material;
};
}

namespace YAML {
template <>
struct convert<VKPT::Mesh> {
    static Node encode(const VKPT::Mesh &rhs) {
        Node node;
        node["Translation"] = rhs.translation;
        node["Scale"] = rhs.scale;
        node["Rotation"] = rhs.rotation;
        node["Material"] = rhs.material;
        return node;
    }

    static bool decode(const Node &node, VKPT::Mesh &rhs) {
        if (!node.IsMap() || !node["File"]) {
            return false;
        }

        if (node["Translation"]) {
            rhs.translation[0] = node["Translation"][0].as<float>();
            rhs.translation[1] = node["Translation"][1].as<float>();
            rhs.translation[2] = node["Translation"][2].as<float>();
        }

        if (node["Scale"]) {
            rhs.scale[0] = node["Scale"][0].as<float>();
            rhs.scale[1] = node["Scale"][1].as<float>();
            rhs.scale[2] = node["Scale"][2].as<float>();
        }

        if (node["Rotation"]) {
            rhs.rotation[0] = node["Rotation"][0].as<float>();
            rhs.rotation[1] = node["Rotation"][1].as<float>();
            rhs.rotation[2] = node["Rotation"][2].as<float>();
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
    vec3 translation;
    uint triangleCount;
    vec3 scale;
    uint startIndex;
    vec3 rotation;
    AABB bounds;
    Material material;
};

/* ---------- GLSL ---------- */
#endif
#endif
