#ifndef CORE_OBJECTS_MATERIAL_H
#define CORE_OBJECTS_MATERIAL_H

#ifdef __cplusplus
/* ---------- C++ ---------- */
#include "Math.h"
#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>
using namespace glm;

namespace VKPT {

struct alignas(16) Material {
    vec3 baseColor = { 1.0f, 1.0f, 1.0f };
    float metallic = 0.0f;
    vec3 specularTint = { 1.0f, 1.0f, 1.0f };
    float emissionStrength = 0.0f;
    vec3 emissionColor = { 1.0f, 1.0f, 1.0f };
    float roughness = 0.5f;
    vec2 IOR = { 1.5f, 0.5f };
    float transmission = 0.0f;

    // Loaded dynamically by GLTFLoader, not saved
    int32_t baseColorTextureIndex = -1;
    int32_t metallicRoughnessTextureIndex = -1;
    int32_t normalTextureIndex = -1;
    int32_t doubleSided = 0;
};

} // namespace VKPT

namespace YAML {
template <>
struct convert<VKPT::Material> {
    static Node encode(const VKPT::Material &rhs) {
        Node node;
        node["BaseColor"]             = rhs.baseColor;
        node["SpecularTint"]          = rhs.specularTint;
        node["Metallic"]              = rhs.metallic;
        node["EmissionColor"]         = rhs.emissionColor;
        node["EmissionStrength"]      = rhs.emissionStrength;
        node["IOR"]                   = rhs.IOR;
        node["Roughness"]             = rhs.roughness;
        node["Transmission"]          = rhs.transmission;
        return node;
    }

    static bool decode(const Node &node, VKPT::Material &rhs) {
        if (!node.IsMap())
            return false;

        if (node["BaseColor"]) {
            rhs.baseColor[0] = node["BaseColor"][0].as<float>();
            rhs.baseColor[1] = node["BaseColor"][1].as<float>();
            rhs.baseColor[2] = node["BaseColor"][2].as<float>();
        }

        if (node["SpecularTint"]) {
            rhs.specularTint[0] = node["SpecularTint"][0].as<float>();
            rhs.specularTint[1] = node["SpecularTint"][1].as<float>();
            rhs.specularTint[2] = node["SpecularTint"][2].as<float>();
        }

        if (node["Metallic"])
            rhs.metallic = node["Metallic"].as<float>();

        if (node["EmissionColor"]) {
            rhs.emissionColor[0] = node["EmissionColor"][0].as<float>();
            rhs.emissionColor[1] = node["EmissionColor"][1].as<float>();
            rhs.emissionColor[2] = node["EmissionColor"][2].as<float>();
        }

        if (node["EmissionStrength"])
            rhs.emissionStrength = node["EmissionStrength"].as<float>();

        if (node["IOR"]) {
            rhs.IOR[0] = node["IOR"][0].as<float>();
            rhs.IOR[1] = node["IOR"][1].as<float>();
        }

        if (node["Roughness"])
            rhs.roughness = node["Roughness"].as<float>();

        if (node["Transmission"])
            rhs.transmission = node["Transmission"].as<float>();

        return true;
    }
};
} // namespace YAML

/* ---------- C++ ---------- */
#else
/* ---------- GLSL ---------- */

// Corrected GLSL Material struct that mirrors the C++ layout.
// (If using this in a uniform block, be sure to declare "layout(std140)" on the block.)
//
// Note:
// - GLSL does not pack booleans in std140; here we use an int (0 for false, nonzero for true)
// - The order of members is chosen to match the C++ version.
struct Material {
    vec3 baseColor;
    float metallic;
    vec3 specularTint;
    float emissionStrength;
    vec3 emissionColor;
    float roughness;
    vec2 IOR;
    float transmission;

    // Loaded dynamically by GLTFLoader, not saved
    int baseColorTextureIndex;
    int metallicRoughnessTextureIndex;
    int normalTextureIndex;
    int doubleSided;
};

#endif
#endif
