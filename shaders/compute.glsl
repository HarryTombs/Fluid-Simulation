#version 430
layout(local_size_x = 16, local_size_y = 16) in;

layout(binding = 0) uniform sampler2D inputTex;
layout(rgba32f, binding = 1) uniform writeonly image2D outputTex;

uniform ivec2 Resolution;
uniform ivec2 mousePos; 
uniform bool mousePress;

vec2 normalizeCoords(ivec2 pos)
{
    return (vec2(pos)+0.5) / vec2(Resolution);
}

vec4 Field(vec2 uv) 
{
    return texture(inputTex,uv);
}

void main() 
{
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    vec2 uv = normalizeCoords(pos);

    vec4 Energy = Field(uv);

    float offset = 1.0 / float(Resolution.x);

    vec4 pX = Field(uv + vec2(offset, 0));
    vec4 nX = Field(uv - vec2(offset, 0));
    vec4 pY = Field(uv + vec2(0, offset));
    vec4 nY = Field(uv - vec2(0, offset));

    // Rule 2: Disordered energy (blue) diffuses
    Energy.b = (pX.b + pY.b + nX.b + nY.b) / 4.0;

    // Rule 3: Order emerges from disordered gradients
    vec2 Force;
    Force.x = nX.b - pX.b;
    Force.y = nY.b - pY.b;
    Energy.xy += Force / 4.0;

    // Rule 4: Disorder created from flow
    Energy.b += (nX.x - pX.x + nY.y - pY.y) / 4.0;

    // Gravity
    Energy.y -= Energy.a / 300.0;

    // Mass conservation
    Energy.a += (nX.x * nX.a - pX.x * pX.a + nY.y * nY.a - pY.y * pY.a) / 4.0;

    if (pos.x < 10 || pos.y < 10 || Resolution.x - pos.x < 10 || Resolution.y - pos.y < 10) {
        Energy.xy *= vec2(0.0);
    }



    int radius = 6;
    if (mousePress && distance(vec2(pos), vec2(mousePos)) < 10.0) {
        Energy.a = 1.0; // inject mass
    }

    // Clamp velocity to prevent explosive growth
    Energy.xy = clamp(Energy.xy, vec2(-10.0), vec2(10.0));

    Energy.b = clamp(Energy.b, 0.0, 4.0);
    Energy.a = clamp(Energy.a, 0.0, 4.0);

    imageStore(outputTex, pos, Energy); 
}   