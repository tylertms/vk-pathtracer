#ifndef SPHERE_GLSL
#define SPHERE_GLSL

#include "Common.glsl"

struct Sphere {
    vec3 center;
    float radius;
    Material material;
};

HitPayload rayHitSphere(Ray ray, Sphere sphere) {
  HitPayload hit;
  hit.didHit = false;
  hit.time = 1e20;
  hit.point = vec3(0.0);
  hit.normal = vec3(0.0);

  vec3 offset = sphere.center - ray.origin;
  float a = dot(ray.dir, ray.dir);
  float h = dot(ray.dir, offset);
  float c = dot(offset, offset) - sphere.radius * sphere.radius;
  float discriminant = h * h - a * c;

  float hasHit = step(0.0, discriminant);
  float sqrtDiscriminant = sqrt(max(discriminant, 0.0));
  float t = (h - sqrtDiscriminant) / a;
  float validT = step(1e-6, t);

  float hitCondition = hasHit * validT;

  hit.didHit = hitCondition > 0.5;
  hit.time = mix(hit.time, t, hitCondition);
  hit.point = mix(hit.point, ray.origin + ray.dir * t, hitCondition);
  hit.normal = mix(hit.normal, normalize(hit.point - sphere.center), hitCondition);

  return hit;
}

#endif
