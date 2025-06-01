#version 430
layout(local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 0) uniform readonly image2D inputTex;
layout(rgba32f, binding = 1) uniform writeonly image2D outputTex;

uniform ivec2 Resolution;

void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    if (any(greaterThanEqual(pos, Resolution))) return;

    vec4 c = imageLoad(inputTex, pos);
    vec4 up    = imageLoad(inputTex, clamp(pos + ivec2(0,1), ivec2(0), Resolution - 1));
    vec4 down  = imageLoad(inputTex, clamp(pos + ivec2(0,-1), ivec2(0), Resolution - 1));
    vec4 left  = imageLoad(inputTex, clamp(pos + ivec2(-1,0), ivec2(0), Resolution - 1));
    vec4 right = imageLoad(inputTex, clamp(pos + ivec2(1,0), ivec2(0), Resolution - 1));

    vec4 avg = (up + down + left + right) / 4.0;
    vec4 result = mix(c, avg, 0.25); // tweak for diffusion speed

    imageStore(outputTex, pos, result);
}
