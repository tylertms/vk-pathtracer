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
    vec4 prevColor = imageLoad(accumulationImage, fragCoord);
    
    vec4 sampleColor;
    if (scene.framesRendered < 100) sampleColor = vec4(1, 1, 1, 1);
    else {
        sampleColor = vec4(0, 0, 0, 1);
    }

    outColor = mix(prevColor, sampleColor, 1.0f / (scene.framesRendered + 1));
    
    imageStore(accumulationImage, fragCoord, outColor);

}
