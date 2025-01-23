#version 460

/* --------------------------------------*/
#include "Objects/Sphere.glsl"
#include "Objects/Ray.glsl"
/* --------------------------------------*/
layout (location = 0) in vec2 fragUV;

layout (binding = 0) readonly uniform _ { Scene scene; };
layout (binding = 1, rgba32f) uniform image2D accumulationImage;
/* --------------------------------------*/
layout (location = 0) out vec4 outColor;
/* --------------------------------------*/

void main() {
    ivec2 texCoord = ivec2(gl_FragCoord);

    Ray ray = generateRay(fragUV, scene.camera.aspectRatio);
    Sphere sphere;
    sphere.center = vec3(0);
    sphere.radius = 0.5;

    HitPayload hit = rayHitSphere(ray, sphere);

    vec4 sampleColor = hit.didHit ? vec4(1) : vec4(0);

    vec4 prevColor = imageLoad(accumulationImage, texCoord);
    outColor = mix(prevColor, sampleColor, 1.0 / (scene.framesRendered + 1));
    imageStore(accumulationImage, texCoord, outColor);
}
