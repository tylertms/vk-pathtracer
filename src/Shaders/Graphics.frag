#version 460
/* --------------------------------------*/
#extension GL_ARB_fragment_shader_interlock : require
/* --------------------------------------*/
#include "Core/Common.glsl"
#include "../Core/Types/Scene.h"
/* --------------------------------------*/
layout (pixel_interlock_unordered) in;
layout (location = 0) in vec2 fragUV;

layout (binding = 0, std140) readonly uniform SceneUniform {
    Camera camera;
    Sphere spheres[MAX_SPHERES];
    Mesh meshes[MAX_MESHES];

    uint numSpheres;
    uint numMeshes;
    uint numTriangles;
    uint numBVHs;
    uint framesRendered;
}
scene;

layout (binding = 1, rgba32f) coherent uniform image2D accumulationImage;

layout (binding = 2, std430) readonly buffer SceneStorage {
    BVH bvhs[MAX_BVHS];
    Triangle triangles[MAX_TRIANGLES];
};

/* --------------------------------------*/
layout (location = 0) out vec4 outColor;
/* --------------------------------------*/
#include "Core/Scene.glsl"
#include "Core/Ray.glsl"
/* --------------------------------------*/

// #define DEBUG_TRI_TESTS

void main() {
    ivec2 texCoord = ivec2(gl_FragCoord);
    uint state = (texCoord.y * 814793u + texCoord.x * 27u) ^ (scene.framesRendered * 102943u) ^ scene.framesRendered;

    uint stats[2] = { 0, 0 };

    vec3 totalLight = vec3(0);
    for (uint i = 0; i < scene.camera.samplesPerPixel; i++) {
        Ray ray = generateRay(fragUV, scene.camera, state);
        totalLight += traceRay(ray, scene.camera.maxBounces, state, stats);
    }
    totalLight /= scene.camera.samplesPerPixel;

    vec4 newColor = vec4(totalLight, 1);

#ifdef DEBUG_BOX_TESTS
    float boxTests = stats[0] / 500.0;
    vec3 boxTestColor = boxTests > 1 ? vec3(1, 0, 0) : vec3(boxTests);
    newColor = vec4(boxTestColor, 1);
#endif

#ifdef DEBUG_TRI_TESTS
    float triTests = stats[1] / 50.0;
    vec3 triTestColor = triTests > 1 ? vec3(1, 0, 0) : vec3(triTests);
    newColor = vec4(triTestColor, 1);
#endif

    beginInvocationInterlockARB();
    vec4 prevColor = imageLoad(accumulationImage, texCoord);
    newColor = mix(prevColor, newColor, 1.0 / (scene.framesRendered + 1));
    imageStore(accumulationImage, texCoord, newColor);
    endInvocationInterlockARB();

    outColor = sqrt(newColor);
}
