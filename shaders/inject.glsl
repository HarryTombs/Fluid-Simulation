#version 430
layout(local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 0) uniform image2D outputTex;

uniform ivec2 Resolution;
uniform vec2 mousePos; 
uniform bool mousePress;
uniform vec2 mouseDelta;

void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);




    if (mousePress) 
    {
        vec2 fragcoord = vec2(pos) / vec2(Resolution);
        float dist = distance(fragcoord,mousePos);
        float influence = exp(-dist * 10000.0);

        vec4 vel = imageLoad(outputTex, pos);
        vel.xy += mouseDelta * influence * 10.0;
        imageStore(outputTex, pos, vel);
    }

}