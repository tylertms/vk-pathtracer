#version 460
/* --------------------------------------*/
#extension GL_ARB_fragment_shader_interlock : require
/* --------------------------------------*/
#include "Core/Common.glsl"
#include "../Core/Types/SceneObject.h"
/* --------------------------------------*/
layout(pixel_interlock_unordered) in;
layout (location = 0) in vec2 fragUV;
layout (binding = 0, std140) readonly uniform SceneObject {
    Camera camera;
    Sphere spheres[MAX_SPHERES];
    Triangle triangles[MAX_TRIANGLES];
    Mesh meshes[MAX_MESHES];
    uint numSpheres;
    uint numTriangles;
    uint numMeshes;
    uint framesRendered;
} scene;
layout (binding = 1, rgba32f) coherent uniform image2D accumulationImage;
layout (binding = 2, rgba8) coherent uniform image2D outputImage;
/* --------------------------------------*/
layout (location = 0) out vec4 outColor;
/* --------------------------------------*/
#include "Core/Scene.glsl"
#include "Core/Ray.glsl"
/* --------------------------------------*/

void main() {
    ivec2 texCoord = ivec2(gl_FragCoord);
    uint state = (texCoord.y * 814793u + texCoord.x * 27u) ^ (scene.framesRendered * 102943u) ^ scene.framesRendered;

    Ray ray = generateRay(fragUV, scene.camera.aspectRatio);

    vec3 totalLight = vec3(0);
    for (uint i = 0; i < SAMPLES_PER_PIXEL; i++) {
        totalLight += traceRay(ray, state);
    }
    totalLight /= SAMPLES_PER_PIXEL;

    vec4 newColor = vec4(totalLight, 1);

    beginInvocationInterlockARB();
    vec4 prevColor = imageLoad(accumulationImage, texCoord);
    newColor = mix(prevColor, newColor, 1.0 / (scene.framesRendered + 1));
    imageStore(accumulationImage, texCoord, newColor);
    endInvocationInterlockARB();

    outColor = sqrt(newColor);
    imageStore(outputImage, texCoord, outColor);
}
