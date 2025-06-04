#version 430

layout(local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 0) uniform readonly image2D velocityTex;
layout(rgba32f, binding = 1) uniform writeonly image2D newDye;

uniform sampler2D oldDye;

uniform float deltaTime;
uniform ivec2 Resolution;
float dissipation = 0.99;


void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    if (pos.x >= Resolution.x || pos.y >= Resolution.y) return;

    vec2 uv = (vec2(pos)+0.5) / Resolution;
    vec2 vel = imageLoad(velocityTex, pos).xy;

    // finding previous UV co-ordinate for Semi-Lagrangian advection

    vec2 prevUV = uv - (deltaTime * vel / Resolution);
    vec4 advected = textureLod(oldDye,prevUV,0.0);

    advected *= dissipation;

    // new dye positon outputed to fragment shader
    
    imageStore(newDye, pos, advected);
}