#version 430
layout(local_size_x = 1, local_size_y = 1) in;

layout(rgba32f, binding = 0) uniform readonly image2D inputTex;
layout(rgba32f, binding = 1) uniform writeonly image2D outputTex;

void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);

    vec4 centre = imageLoad(inputTex, pos);
    vec4 up = imageLoad(inputTex, ivec2(pos + vec2(0,1)));
    vec4 down = imageLoad(inputTex, ivec2(pos + vec2(0,-1)));
    vec4 left = imageLoad(inputTex, ivec2(pos + vec2(-1,0)));
    vec4 right = imageLoad(inputTex, ivec2(pos + vec2(1,0)));

    vec4 average = (up + down + left + right) / 4.0;
    vec4 result = mix(centre,average,0.25);

    imageStore(outputTex, pos, result); 
}   