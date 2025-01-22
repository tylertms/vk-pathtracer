#version 460

/* --------------------------------------*/
#include "Scene.glsl"
/* --------------------------------------*/
layout (location = 0) in vec2 fragUV;
layout (location = 1) in vec2 texCoord;

layout (binding = 0) readonly uniform _ { Scene scene; };
layout (binding = 1, rgba32f) uniform image2D accumulationImage;
layout (binding = 2, rgba8) uniform image2D renderImage;
/* --------------------------------------*/
layout (location = 0) out vec4 fragColor;
/* --------------------------------------*/

void main() {
    fragColor = vec4(1.0, 0.0, 1.0, 1.0);
}
