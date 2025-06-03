#version 430

layout(local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 0) uniform readonly image2D velocityTex;
layout(r32f, binding = 1) uniform readonly image2D pressureTexIn;
layout(rgba32f, binding = 2) uniform writeonly image2D outputTex;

uniform ivec2 Resolution;

void main()
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    if (pos.x >= Resolution.x || pos.y >= Resolution.y) return;

    float pX = imageLoad(pressureTexIn, pos + ivec2(1,0)).x;
    float nX = imageLoad(pressureTexIn, pos - ivec2(1,0)).x;
    float pY = imageLoad(pressureTexIn, pos + ivec2(0,1)).x;
    float nY = imageLoad(pressureTexIn, pos - ivec2(0,1)).x;

    vec4 velocityImg = imageLoad(velocityTex,pos);
    vec2 velocity = velocityImg.xy;
    vec2 gradient = vec2(pX - nX, pY - nY) * 0.5;

    velocity -= gradient;
    imageStore(outputTex,pos,vec4(velocity,0.0,1.0));
}
