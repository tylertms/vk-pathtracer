#ifndef CORE_SPHERE_GLSL
#define CORE_SPHERE_GLSL

#include "Common.glsl"

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

#endif
