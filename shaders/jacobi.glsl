#version 430

layout(local_size_x = 16, local_size_y = 16) in;

layout(r32f, binding = 0) uniform readonly image2D pressureIn;
layout(r32f, binding = 1) uniform writeonly image2D pressureOut;
layout(r32f, binding = 2) uniform readonly image2D divergence;

uniform ivec2 Resolution;

void main() {
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    if (coord.x <= 0 || coord.y <= 0 || coord.x >= Resolution.x - 1 || coord.y >= Resolution.y - 1) {
        imageStore(pressureOut, coord, vec4(0));
        return;
    }

    float L = imageLoad(pressureIn, coord + ivec2(-1, 0)).r;
    float R = imageLoad(pressureIn, coord + ivec2(1, 0)).r;
    float B = imageLoad(pressureIn, coord + ivec2(0, -1)).r;
    float T = imageLoad(pressureIn, coord + ivec2(0, 1)).r;
    float D = imageLoad(divergence, coord).r;

    float pressure = (L + R + B + T - D) * 0.25;
    imageStore(pressureOut, coord, vec4(pressure));
}