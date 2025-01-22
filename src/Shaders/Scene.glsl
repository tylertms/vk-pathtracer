struct Camera {
    vec3 lookFrom;
    float vfov;
    vec3 lookAt;
    float aperture;
    float aspectRatio;
};

struct Scene {
    Camera cam;
};