#version 460

/* --------------------------------------*/
layout (location = 0) out vec2 fragUV;
/* --------------------------------------*/

/*
 * Source: https://www.saschawillems.de/blog/2016/08/13/vulkan-tutorial-on-rendering-a-fullscreen-quad-without-buffers/
 * Draws a fullscreen triangle over the viewport
 * fragUV: [-1,1]
*/
void main() {
    fragUV = 2. * vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2) - 1.;
    gl_Position = vec4(fragUV, 0., 1.);
}