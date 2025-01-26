
#ifndef CORE_OBJECTS_MATERIAL_H
#define CORE_OBJECTS_MATERIAL_H

#ifdef __cplusplus
/* ---------- C++ ---------- */
#include "Vec3.h"
#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>
using namespace glm;

namespace VKPT {
struct alignas(16) Material {
    vec3 color = { 1.0, 1.0, 1.0 };
    float emissionStrength = 0.0;
    vec3 emissionColor = { 1.0, 1.0, 1.0 };
};
}

namespace YAML {
template <>
struct convert<VKPT::Material> {
    static Node encode(const VKPT::Material &rhs) {
        Node node;
        node["Color"] = rhs.color;
        node["EmissionColor"] = rhs.emissionColor;
        node["EmissionStrength"] = rhs.emissionStrength;
        return node;
    }

    static bool decode(const Node &node, VKPT::Material &rhs) {
        if (!node.IsMap()) {
            return false;
        }

        if (node["Color"]) {
            rhs.color[0] = node["Color"][0].as<float>();
            rhs.color[1] = node["Color"][1].as<float>();
            rhs.color[2] = node["Color"][2].as<float>();
        }

        if (node["EmissionColor"]) {
            rhs.emissionColor[0] = node["EmissionColor"][0].as<float>();
            rhs.emissionColor[1] = node["EmissionColor"][1].as<float>();
            rhs.emissionColor[2] = node["EmissionColor"][2].as<float>();
        }

        if (node["EmissionStrength"]) {
            rhs.emissionStrength = node["EmissionStrength"].as<float>();
        }

        return true;
    }
};
} // namespace YAML

/* ---------- C++ ---------- */
#else
/* ---------- GLSL ---------- */

struct Material {
    vec3 color;
    float emissionStrength;
    vec3 emissionColor;
};

/* ---------- GLSL ---------- */
#endif
#endif
