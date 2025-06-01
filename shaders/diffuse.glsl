#version 430
layout(local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 0) writeonly uniform image2D outputTex;

uniform ivec2 uResolution;

void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    
    imageStore(outputTex, pos, vec4(1.0, 0.0, 0.0, 1.0));
}