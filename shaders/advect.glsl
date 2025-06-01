#version 430
layout(local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 0) uniform readonly image2D inputTex;
layout(rgba32f, binding = 1) uniform writeonly image2D outputTex;

uniform ivec2 Resolution;
uniform float deltaTime;

vec4 sampleField(vec2 pos) {
    ivec2 p = ivec2(clamp(pos, vec2(0), vec2(Resolution - 1)));
    return imageLoad(inputTex, p);
}

void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    if (any(greaterThanEqual(pos, Resolution))) return;

    vec2 velocity = imageLoad(inputTex, pos).xy;
    vec2 prevPos = vec2(pos) - velocity * deltaTime * vec2(Resolution);
    vec4 advected = sampleField(prevPos);
    imageStore(outputTex, pos, advected);
}