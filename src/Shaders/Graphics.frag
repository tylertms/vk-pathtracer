#version 460

/* --------------------------------------*/
#include "Scene.glsl"
/* --------------------------------------*/
layout (location = 0) in vec2 fragUV;

layout (binding = 0) readonly uniform _ { Scene scene; };
layout (binding = 1, rgba32f) uniform image2D accumulationImage;
/* --------------------------------------*/
layout (location = 0) out vec4 outColor;
/* --------------------------------------*/

void main() {
    ivec2 texCoord = ivec2(gl_FragCoord);
    vec4 prevColor = imageLoad(accumulationImage, texCoord);
    
    vec4 sampleColor = vec4(0);
    if (abs(texCoord.x - 400) < 10 + scene.framesRendered && abs(texCoord.y - 300) < 10 + scene.framesRendered) sampleColor = vec4(1);

    outColor = mix(prevColor, sampleColor, 1.0f / (scene.framesRendered + 1));
    
    imageStore(accumulationImage, texCoord, outColor);

}
