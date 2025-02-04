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
layout (binding = 2) uniform sampler2D envSampler;

layout (binding = 3, std430) readonly buffer SceneStorage {
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
        Ray ray = generateRay(fragUV, state);
        totalLight += traceRay(ray, scene.camera.maxBounces, state, stats);
    }
    totalLight /= scene.camera.samplesPerPixel;

#ifdef DEBUG_BOX_TESTS
    float boxTests = stats[0] / 500.0;
    vec3 boxTestColor = boxTests > 1 ? vec3(1, 0, 0) : vec3(boxTests);
    outColor = vec4(boxTestColor, 1.f);
    return;
#endif

#ifdef DEBUG_TRI_TESTS
    float triTests = stats[1] / 50.0;
    vec3 triTestColor = triTests > 1 ? vec3(1, 0, 0) : vec3(triTests);
    outColor = vec4(triTestColor, 1.f);
    return;
#endif

    beginInvocationInterlockARB();
    vec3 prevColor = imageLoad(accumulationImage, texCoord).rgb;
    vec3 newColor = mix(prevColor, totalLight, 1.0 / (scene.framesRendered + 1));
    imageStore(accumulationImage, texCoord, vec4(newColor, 1.f));
    endInvocationInterlockARB();

    vec3 hdrColor = newColor * scene.camera.exposure;
    vec3 toneMappedColor = toneMapACES(hdrColor);
    vec3 gammaCorrectedColor = pow(toneMappedColor, vec3(1.0 / 2.2));

    outColor = vec4(gammaCorrectedColor, 1.0);
}
