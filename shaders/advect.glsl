#version 430

layout(local_size_x = 8, local_size_y = 8) in;

layout(rgba32f, binding = 0) readonly uniform image2D quantity;  // quantity to advect (e.g., velocity or density)
layout(rgba32f, binding = 1) readonly uniform image2D velocity;  // velocity field
layout(rgba32f, binding = 2) writeonly uniform image2D outQuantity; // output

uniform ivec2 Resolution;
uniform float deltaTime;

void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    if (pos.x >= Resolution.x || pos.y >= Resolution.y) return;

    vec2 uv = vec2(pos) / vec2(Resolution);

    // Sample velocity at this position
    vec2 vel = imageLoad(velocity, pos).xy;

    // Trace backwards in time
    vec2 prevUV = uv - vel * deltaTime;

    // Clamp to stay within bounds
    prevUV = clamp(prevUV, vec2(0.0), vec2(1.0));

    // Convert UV to pixel space
    ivec2 prevPos = ivec2(prevUV * vec2(Resolution));

    // Load from previous position
    vec4 advected = imageLoad(quantity, prevPos);

    imageStore(outQuantity, pos, advected);
}
