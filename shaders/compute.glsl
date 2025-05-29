#version 430
layout(local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 0) uniform readonly image2D inputTex;
layout(rgba32f, binding = 1) uniform writeonly image2D outputTex;

void main() {
    vec4 value = vec4(0.0,0.0,0.0,1.0);
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);

    value.x = float(pos.x)/(gl_NumWorkGroups.x);
    value.y = float(pos.y)/(gl_NumWorkGroups.y);
    imageStore(outputTex, pos, value); // write green
}