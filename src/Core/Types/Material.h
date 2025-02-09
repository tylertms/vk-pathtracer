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
    vec3  baseColor         = { 1.0f, 1.0f, 1.0f };
    float alpha             = 1.0f;
    vec3  specularTint      = { 1.0f, 1.0f, 1.0f };
    float specular          = 0.5f;
    vec3  emissionColor     = { 1.0f, 1.0f, 1.0f };
    float emissionStrength  = 0.0f;
    vec3  coatTint          = { 1.0f, 1.0f, 1.0f };
    float coatWeight        = 0.0f;
    vec2  IOR               = { 1.5f, 0.5f };
    float roughness         = 0.5f;
    float metallic          = 0.0f;
    float transmission      = 0.0f;
    float transmissionRoughness = 0.0f;
    float coatRoughness     = 0.0f;
    float coatIOR           = 1.5f;
};

} // namespace VKPT

namespace YAML {
template <>
struct convert<VKPT::Material> {
    static Node encode(const VKPT::Material &rhs) {
        Node node;
        node["BaseColor"]             = rhs.baseColor;
        node["Alpha"]                 = rhs.alpha;
        node["SpecularTint"]          = rhs.specularTint;
        node["Specular"]              = rhs.specular;
        node["EmissionColor"]         = rhs.emissionColor;
        node["EmissionStrength"]      = rhs.emissionStrength;
        node["CoatTint"]              = rhs.coatTint;
        node["CoatWeight"]            = rhs.coatWeight;
        node["IOR"]                   = rhs.IOR;
        node["Roughness"]             = rhs.roughness;
        node["Metallic"]              = rhs.metallic;
        node["Transmission"]          = rhs.transmission;
        node["TransmissionRoughness"] = rhs.transmissionRoughness;
        node["CoatRoughness"]         = rhs.coatRoughness;
        node["CoatIOR"]               = rhs.coatIOR;
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

        if (node["Alpha"])
            rhs.alpha = node["Alpha"].as<float>();

        if (node["SpecularTint"]) {
            rhs.specularTint[0] = node["SpecularTint"][0].as<float>();
            rhs.specularTint[1] = node["SpecularTint"][1].as<float>();
            rhs.specularTint[2] = node["SpecularTint"][2].as<float>();
        }

        if (node["Specular"])
            rhs.specular = node["Specular"].as<float>();

        if (node["EmissionColor"]) {
            rhs.emissionColor[0] = node["EmissionColor"][0].as<float>();
            rhs.emissionColor[1] = node["EmissionColor"][1].as<float>();
            rhs.emissionColor[2] = node["EmissionColor"][2].as<float>();
        }

        if (node["EmissionStrength"])
            rhs.emissionStrength = node["EmissionStrength"].as<float>();

        if (node["CoatTint"]) {
            rhs.coatTint[0] = node["CoatTint"][0].as<float>();
            rhs.coatTint[1] = node["CoatTint"][1].as<float>();
            rhs.coatTint[2] = node["CoatTint"][2].as<float>();
        }

        if (node["CoatWeight"])
            rhs.coatWeight = node["CoatWeight"].as<float>();

        if (node["IOR"]) {
            rhs.IOR[0] = node["IOR"][0].as<float>();
            rhs.IOR[1] = node["IOR"][1].as<float>();
        }

        if (node["Roughness"])
            rhs.roughness = node["Roughness"].as<float>();

        if (node["Metallic"])
            rhs.metallic = node["Metallic"].as<float>();

        if (node["Transmission"])
            rhs.transmission = node["Transmission"].as<float>();

        if (node["TransmissionRoughness"])
            rhs.transmissionRoughness = node["TransmissionRoughness"].as<float>();

        if (node["CoatRoughness"])
            rhs.coatRoughness = node["CoatRoughness"].as<float>();

        if (node["CoatIOR"])
            rhs.coatIOR = node["CoatIOR"].as<float>();

        return true;
    }
};
} // namespace YAML

/* ---------- C++ ---------- */
#else
/* ---------- GLSL ---------- */

struct Material {
    vec3 baseColor;
    float alpha;
    vec3 specularTint;
    float specular;
    vec3 emissionColor;
    float emissionStrength;
    vec3 coatTint;
    float coatWeight;
    vec2 IOR;
    float roughness;
    float metallic;
    float transmission;
    float transmissionRoughness;
    float coatRoughness;
    float coatIOR;
};

/* ---------- GLSL ---------- */
#endif
#endif
