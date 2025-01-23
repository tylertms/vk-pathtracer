struct Camera {
    vec3 lookFrom;
    float vfov;
    vec3 lookAt;
    float aperture;
    float aspectRatio;
};

struct Material {
    vec3 color;
    float emissionStrength;
    vec3 emissionColor;
};

struct Sphere {
    vec3 center;
    float radius;
    Material material;
};

struct Scene {
    uint framesRendered;
    Camera cam;
};
