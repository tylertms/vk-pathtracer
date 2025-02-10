#ifndef CORE_OBJECTS_SPHERE_H
#define CORE_OBJECTS_SPHERE_H

#include "../Constants.h"
#include "Material.h"

#ifdef __cplusplus
/* ---------- C++ ---------- */
#include <glm/glm.hpp>
using namespace glm;

namespace VKPT {
struct alignas(16) Sphere {
    vec3 center = {0.0, 0.0, 0.0};
    float radius = 0.5;
    uint32_t materialIndex = 0;
};
} // namespace VKPT

namespace YAML {
template <>
struct convert<VKPT::Sphere> {
    static Node encode(const VKPT::Sphere &rhs) {
        Node node;
        node["Center"] = rhs.center;
        node["Radius"] = rhs.radius;
        node["MaterialIndex"] = rhs.materialIndex;
        return node;
    }

    static bool decode(const Node &node, VKPT::Sphere &rhs) {
        if (!node.IsMap()) {
            return false;
        }

        if (node["Center"]) {
            rhs.center[0] = node["Center"][0].as<float>();
            rhs.center[1] = node["Center"][1].as<float>();
            rhs.center[2] = node["Center"][2].as<float>();
        }

        if (node["Radius"]) {
            rhs.radius = node["Radius"].as<float>();
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

struct Sphere {
    vec3 center;
    float radius;
    uint materialIndex;
};

HitPayload rayHitSphere(Ray ray, Sphere sphere) {
    HitPayload hit;
    hit.didHit = false;

    vec3 offset = ray.origin - sphere.center;

    float a = dot(ray.dir, ray.dir);
    float h = dot(offset, ray.dir);
    float c = dot(offset, offset) - sphere.radius * sphere.radius;

    float discriminant = h * h - a * c;
    if (discriminant < 0) {
        return hit;
    }

    float sqrtDisc = sqrt(discriminant);

    float t1 = (-h - sqrtDisc) / a;
    float t2 = (-h + sqrtDisc) / a;

    float distance = 0.0;
    if (t1 >= DST_EPSILON) {
        distance = t1;
    } else if (t2 >= DST_EPSILON) {
        distance = t2;
    } else {
        return hit;
    }

    hit.didHit = true;
    hit.distance = distance;
    hit.point = ray.origin + ray.dir * distance;
    hit.normal = normalize(hit.point - sphere.center);
    return hit;
}

/* ---------- GLSL ---------- */
#endif
#endif
