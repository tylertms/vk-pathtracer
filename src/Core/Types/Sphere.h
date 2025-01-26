#ifndef CORE_OBJECTS_SPHERE_H
#define CORE_OBJECTS_SPHERE_H

#include "Material.h"
#include "../Constants.h"

#ifdef __cplusplus
/* ---------- C++ ---------- */
#include <glm/glm.hpp>
using namespace glm;

namespace VKPT {
struct alignas(16) Sphere {
    vec3 center = { 0.0, 0.0, 0.0 };
    float radius = 0.5;
    Material material;
};
}

namespace YAML {
template <>
struct convert<VKPT::Sphere> {
    static Node encode(const VKPT::Sphere &rhs) {
        Node node;
        node["Center"] = rhs.center;
        node["Radius"] = rhs.radius;
        node["Material"] = rhs.material;
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

struct Sphere {
    vec3 center;
    float radius;
    Material material;
};

HitPayload rayHitSphere(Ray ray, Sphere sphere) {
    HitPayload hit;
    hit.didHit = false;

    vec3 offset = ray.origin - sphere.center;

    float a = dot(ray.dir, ray.dir);
    float h = dot(offset, ray.dir);
    float c = dot(offset, offset) - sphere.radius * sphere.radius;

    float discriminant = h * h - a * c;
    if (discriminant >= 0) {
        float distance = (-h - sqrt(discriminant)) / a;

        if (distance >= EPSILON) {
            hit.didHit = true;
            hit.distance = distance;
            hit.point = ray.origin + ray.dir * distance;
            hit.normal = normalize(hit.point - sphere.center);
        }
    }

    return hit;
}

/* ---------- GLSL ---------- */
#endif
#endif
