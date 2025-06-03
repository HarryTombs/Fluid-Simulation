#version 430

layout(local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 0) uniform readonly image2D oldDye;
layout(rgba32f, binding = 1) uniform readonly image2D velocityTex;
layout(rgba32f, binding = 2) uniform writeonly image2D newDye;

uniform float dt;
uniform ivec2 Resolution;
vec2 texelSize = 1.0 / Resolution;


void main() {
    ivec2 Me = ivec2(gl_GlobalInvocationID.xy);
    if (Me.x >= Resolution.x || Me.y >= Resolution.y) return;

    vec2 uv = (vec2(Me) + 0.5) * texelSize;

    vec2 vel = imageLoad(velocityTex, Me).xy;

    // Backtrace
    ivec2 prevUV = ivec2(uv - dt * vel * texelSize);

    vec4 dye = imageLoad(oldDye, prevUV);

    imageStore(newDye, Me, dye);
}