#version 430

layout(local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 0) uniform readonly image2D velocity;
layout(rgba32f, binding = 1) uniform writeonly image2D divergence;

uniform ivec2 Resolution;
uniform float halfrdx;

void main() 
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    if (coord.x >= Resolution.x || coord.y >= Resolution.y) return;

    vec2 vL = imageLoad(velocity, coord + ivec2(-1, 0)).xy;
    vec2 vR = imageLoad(velocity, coord + ivec2(1, 0)).xy;
    vec2 vB = imageLoad(velocity, coord + ivec2(0, -1)).xy;
    vec2 vT = imageLoad(velocity, coord + ivec2(0, 1)).xy;

    float div = halfrdx * ((vR.x - vL.x) + (vT.y - vB.y));
    imageStore(divergence, coord, vec4(div, 0, 0, 1));
}
