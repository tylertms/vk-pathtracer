


#ifndef CORE_OBJECTS_CAMERA_H
#define CORE_OBJECTS_CAMERA_H

#ifdef __cplusplus
/* ---------- C++ ---------- */
#include <glm/glm.hpp>
#include <yaml-cpp/yaml.h>
using namespace glm;

namespace VKPT {
struct alignas(16) Camera {
    vec3 lookFrom;
    float vfov;
    vec3 lookAt;
    float focalDistance = 4.f;
    uvec2 windowSize;
    uint32_t samplesPerPixel = 1;
    uint32_t maxBounces = 8;
    float defocus = 0.f;
    float diverge = 1.f;
};
}

namespace YAML {
template <>
struct convert<VKPT::Camera> {
    static Node encode(const VKPT::Camera &rhs) {
        Node node;
        node["LookFrom"] = rhs.lookFrom;
        node["LookAt"] = rhs.lookAt;
        node["VFOV"] = rhs.vfov;
        node["FocalDistance"] = rhs.focalDistance;
        node["SamplesPerPixel"] = rhs.samplesPerPixel;
        node["MaxBounces"] = rhs.maxBounces;
        node["Defocus"] = rhs.defocus;
        node["Diverge"] = rhs.diverge;
        return node;
    }

    static bool decode(const Node &node, VKPT::Camera &rhs) {
        if (!node.IsMap()) {
            return false;
        }

        if (node["LookFrom"]) {
            rhs.lookFrom[0] = node["LookFrom"][0].as<float>();
            rhs.lookFrom[1] = node["LookFrom"][1].as<float>();
            rhs.lookFrom[2] = node["LookFrom"][2].as<float>();
        }

        if (node["LookAt"]) {
            rhs.lookAt[0] = node["LookAt"][0].as<float>();
            rhs.lookAt[1] = node["LookAt"][1].as<float>();
            rhs.lookAt[2] = node["LookAt"][2].as<float>();
        }

        if (node["VFOV"]) {
            rhs.vfov = node["VFOV"].as<float>();
        }

        if (node["FocalDistance"]) {
            rhs.focalDistance = node["FocalDistance"].as<float>();
        }

        if (node["SamplesPerPixel"]) {
            rhs.samplesPerPixel = node["SamplesPerPixel"].as<uint32_t>();
        }

        if (node["MaxBounces"]) {
            rhs.maxBounces = node["MaxBounces"].as<uint32_t>();
        }

        if (node["Defocus"]) {
            rhs.defocus = node["Defocus"].as<float>();
        }

        if (node["Diverge"]) {
            rhs.diverge = node["Diverge"].as<float>();
        }

        return true;
    }
};
}

/* ---------- C++ ---------- */
#else
/* ---------- GLSL ---------- */

struct Camera {
    vec3 lookFrom;
    float vfov;
    vec3 lookAt;
    float focalDistance;
    uvec2 windowSize;
    uint samplesPerPixel;
    uint maxBounces;
    float defocus;
    float diverge;
};

/* ---------- GLSL ---------- */
#endif
#endif
