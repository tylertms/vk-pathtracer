


#ifndef CORE_OBJECTS_CAMERA_H
#define CORE_OBJECTS_CAMERA_H

#ifdef __cplusplus
/* ---------- C++ ---------- */
#include <glm/glm.hpp>
using namespace glm;

namespace VKPT {
struct alignas(16) Camera {
    vec3 lookFrom;
    float vfov;
    vec3 lookAt;
    float aspectRatio;
};
}

/* ---------- C++ ---------- */
#else
/* ---------- GLSL ---------- */

struct Camera {
    vec3 lookFrom;
    float vfov;
    vec3 lookAt;
    float aspectRatio;
};

/* ---------- GLSL ---------- */
#endif
#endif
