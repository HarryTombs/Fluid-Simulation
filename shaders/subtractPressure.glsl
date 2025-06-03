#version 430

layout(local_size_x = 16, local_size_y = 16) in;

layout(rgba32f,   binding = 0) uniform readonly image2D pressure;
layout(rgba32f, binding = 1) uniform image2D velocity;

uniform ivec2 Resolution;
uniform float halfrdx;

void main() {
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    if (coord.x <= 0 || coord.y <= 0 || coord.x >= Resolution.x - 1 || coord.y >= Resolution.y - 1) return;

    float pL = imageLoad(pressure, coord + ivec2(-1, 0)).r;
    float pR = imageLoad(pressure, coord + ivec2(1, 0)).r;
    float pB = imageLoad(pressure, coord + ivec2(0, -1)).r;
    float pT = imageLoad(pressure, coord + ivec2(0, 1)).r;

    vec2 grad = halfrdx * vec2(pR - pL, pT - pB);
    vec4 vel = imageLoad(velocity, coord);
    vel.xy -= grad;
    imageStore(velocity, coord, vel);
}
