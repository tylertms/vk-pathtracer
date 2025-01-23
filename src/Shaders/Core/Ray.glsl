#include "Common.glsl"


float splitMix32(inout uint state) {
  uint z = (state += 0x9e3779b9);
  z ^= z >> 16;
  z *= 0x21f0aaad;
  z ^= z >> 15;
  z *= 0x735a2d97;
  z ^= z >> 15;
  return z / 4294967295.0;
}

float randValueNormal(inout uint state) {
  float theta = 2 * 3.14159265 * splitMix32(state);
  float rho = sqrt(-2 * log(splitMix32(state)));
  return rho * cos(theta);
}

vec3 randDir(inout uint state) {
  float x = randValueNormal(state);
  float y = randValueNormal(state);
  float z = randValueNormal(state);
  return normalize(vec3(x, y, z));
}

vec3 randHemisphereDir(vec3 normal, inout uint state) {
  vec3 dir = randDir(state);
  return dir * sign(dot(normal, dir));
}


Ray generateRay(vec2 uv, float aspectRatio) {
    uv.x *= aspectRatio;

    Ray ray;
    ray.origin = vec3(0, 0, -6);
    ray.dir = normalize(vec3(uv, -2) - ray.origin);

    return ray;
}

vec3 traceRay(Ray ray, inout uint state) {
    vec3 rayColor = vec3(1);
    vec3 incomingLight = vec3(0);

    for (uint i = 0; i <= MAX_BOUNCES; i++) {
        HitPayload hit = rayHitScene(ray);
        if (!hit.didHit) break;

        ray.origin = hit.point;
        ray.dir = randHemisphereDir(hit.normal, state);

        vec3 emittedLight = hit.material.emissionColor * hit.material.emissionStrength;
        incomingLight += emittedLight * rayColor;
        rayColor *= hit.material.color;
    }

    return incomingLight;
}
