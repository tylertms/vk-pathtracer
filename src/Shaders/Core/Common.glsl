#ifndef CORE_COMMON_GLSL
#define CORE_COMMON_GLSL

#define SAMPLES_PER_PIXEL 1
#define MAX_BOUNCES 8

#define MAX_SPHERES 4
#define MAX_TRIANGLES 2000
#define MAX_MESHES 4

#define EPSILON 1e-5

struct Camera {
    vec3 lookFrom;
    float vfov;
    vec3 lookAt;
    float aspectRatio;
};

struct Ray {
    vec3 origin;
    vec3 dir;
    vec3 inv;
};

struct Material {
    vec3 color;
    float emissionStrength;
    vec3 emissionColor;
};

struct HitPayload {
    bool didHit;
    vec3 point;
    vec3 normal;
    float distance;
    Material material;
};

#endif
