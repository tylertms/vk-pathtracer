#ifndef COMMON_GLSL
#define COMMON_GLSL

#define SAMPLES_PER_PIXEL 10
#define MAX_BOUNCES 8
#define MAX_SPHERES 8

struct Camera {
    vec3 lookFrom;
    float vfov;
    vec3 lookAt;
    float aspectRatio;
};

struct Ray {
    vec3 origin;
    vec3 dir;
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
    float time;
    Material material;
};

#endif
