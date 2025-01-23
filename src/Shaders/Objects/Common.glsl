#ifndef TYPES_GLSL
#define TYPES_GLSL

struct Camera {
    vec3 lookFrom;
    float vfov;
    vec3 lookAt;
    float aperture;
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

struct Scene {
    uint framesRendered;
    Camera camera;
};

struct HitPayload {
    bool didHit;
    vec3 point;
    vec3 normal;
    float time;
    Material material;
};

#endif
