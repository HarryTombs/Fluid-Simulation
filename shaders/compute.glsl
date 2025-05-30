#version 430
layout(local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 0) uniform readonly image2D inputTex;
layout(rgba32f, binding = 1) uniform writeonly image2D outputTex;

uniform ivec2 Resolution;
uniform ivec2 mousePos; 
uniform bool mousePress;

vec4 Field(ivec2 pos) 
{
    pos = clamp(pos, ivec2(0), Resolution - ivec2(1));
    vec2 vel = imageLoad(inputTex, pos).xy;
    ivec2 advectPos = pos - ivec2(vel);
    advectPos = clamp(advectPos, ivec2(0), Resolution - ivec2(1));
    return imageLoad(inputTex, advectPos);
}

void main() 
{
    ivec2 Me = ivec2(gl_GlobalInvocationID.xy);
    vec4 Energy = Field(Me);

    vec4 pX = Field(Me + ivec2(1, 0));
    vec4 nX = Field(Me - ivec2(1, 0));
    vec4 pY = Field(Me + ivec2(0, 1));
    vec4 nY = Field(Me - ivec2(0, 1));

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
    Energy.a += (nX.x * nX.a - pX.x * pX.a + nY.y * nY.a - pY.y * pY.a) / 8.0;

    if (Me.x < 10 || Me.y < 10 || Resolution.x - Me.x < 10 || Resolution.y - Me.y < 10) {
        Energy.xy = vec2(0.0);
    }



    int radius = 6;
    if (mousePress && distance(vec2(Me), vec2(mousePos)) < 10.0) {
        Energy.a = 1.0; // inject mass
    }

    // Clamp velocity to prevent explosive growth
    Energy.xy = clamp(Energy.xy, vec2(-10.0), vec2(10.0));

    // Clamp disorder and mass (blue and alpha)
    Energy.b = clamp(Energy.b, 0.0, 1.0);
    Energy.a = clamp(Energy.a, 0.0, 5.0);  // you can raise this if too weak

    imageStore(outputTex, Me, Energy); 
}   