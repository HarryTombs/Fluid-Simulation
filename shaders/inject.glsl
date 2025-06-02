#version 430
layout(local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 0) uniform image2D densityTex;

uniform ivec2 Resolution;
float densityAmount = 1.0;
int injectX = 250;
int injectY = 250;
float radius = 5;


void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    if (pos.x >= Resolution.x || pos.y >= Resolution.y) return;

    int dx = abs(pos.x - injectX);
    int dy = abs(pos.y - injectY);
    if (dx > radius) return;
    if (dy > radius) return;

    float falloff = 1.0 - float(dx) / radius;
    vec4 current = imageLoad(densityTex, pos);
    current.r += densityAmount * falloff;
    imageStore(densityTex, pos, current);

}