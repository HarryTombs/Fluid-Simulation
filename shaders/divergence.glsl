#version 430

layout(local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 0) uniform readonly image2D velocityTex;
layout(r32f, binding = 1) uniform writeonly image2D DivergenceTex;

uniform ivec2 Resolution;

void main() 
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    if (pos.x >= Resolution.x || pos.y >= Resolution.y) return;

    vec2 pX = imageLoad(velocityTex, pos + ivec2(1,0)).xy;
    vec2 nX = imageLoad(velocityTex, pos - ivec2(1,0)).xy;
    vec2 pY = imageLoad(velocityTex, pos + ivec2(0,1)).xy;
    vec2 nY = imageLoad(velocityTex, pos - ivec2(0,1)).xy;

    // calculated divergence based on in going or out going velocity

    float divergence = 0.5 * ((nX.x - pX.x) + (pY.y - nY.y));
    imageStore(DivergenceTex,pos,vec4(divergence,0.0,0.0,0.0));
}