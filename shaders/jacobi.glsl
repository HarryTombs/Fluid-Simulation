#version 430

layout(local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 0) uniform readonly image2D pressureTexIn;
layout(rgba32f, binding = 1) uniform writeonly image2D pressureTexOut;
layout(r32f, binding = 2) uniform readonly image2D DivergenceTex;

uniform ivec2 Resolution;

void main() 
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    if (pos.x >= Resolution.x || pos.y >= Resolution.y) return;

    // Smooting iterations based on current pressure and calculated divergence

    float pX = imageLoad(pressureTexIn, pos + ivec2(1,0)).x;
    float nX = imageLoad(pressureTexIn, pos - ivec2(1,0)).x;
    float pY = imageLoad(pressureTexIn, pos + ivec2(0,1)).x;
    float nY = imageLoad(pressureTexIn, pos - ivec2(0,1)).x;
    float diver = imageLoad(DivergenceTex, pos).x;

    float pressure = (pX + nX + pY + nY - diver) * 0.25;
    imageStore(pressureTexOut,pos,vec4(pressure,0.0,0.0,0.0));

}