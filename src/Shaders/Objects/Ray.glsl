#include "Common.glsl"

Ray generateRay(vec2 uv, float aspectRatio) {
    uv.x *= aspectRatio;

    Ray ray;
    ray.origin = vec3(0, 0, -6);
    ray.dir = normalize(vec3(uv, -2) - ray.origin);

    return ray;
}
