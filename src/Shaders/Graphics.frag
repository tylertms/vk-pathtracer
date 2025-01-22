#version 460

/* --------------------------------------*/
#include "Scene.glsl"
/* --------------------------------------*/
layout (location = 0) in vec2 fragUV;
layout (location = 1) in vec2 texCoord;

layout (binding = 0) readonly uniform _ { Scene scene; };
layout (binding = 1, rgba32f) uniform image2D accumulationImage;
/* --------------------------------------*/
layout (location = 0) out vec4 outColor;
/* --------------------------------------*/

void main() {
    ivec2 fragCoord = ivec2(gl_FragCoord);
    vec4 fragColor = imageLoad(accumulationImage, fragCoord);
    
    if (scene.framesRendered < 1000) outColor = vec4(1, 1, 1, 1);
    else {
        outColor = vec4(0, 0, 0, 1);
    }

    outColor = mix(fragColor, outColor, 1.f / (scene.framesRendered + 1));
    imageStore(accumulationImage, fragCoord, outColor);
}
