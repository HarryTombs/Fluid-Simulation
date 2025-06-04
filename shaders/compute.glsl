#version 430
layout(local_size_x = 16, local_size_y = 16) in;

uniform sampler2D inVelTex; // sampling non interget values
layout(rgba32f, binding = 0) uniform writeonly image2D outVelTex;
layout(rgba32f, binding = 1) uniform writeonly image2D outDyeTex;

// inputs and Resolution

uniform ivec2 Resolution;
uniform ivec2 mousePos; 
uniform bool mousePress;

// Field converts to UV co-ords and finds the velocity position of the given pixel co-ords

vec4 Field(ivec2 pos) 
{
    pos = clamp(pos, ivec2(0), Resolution - ivec2(1));
    vec2 uv = (vec2(pos) + 0.5) / vec2(textureSize(inVelTex,0));
    vec2 vel = texture(inVelTex, uv).xy;
    vec2 advectPos = uv - vel;
    return texture(inVelTex, advectPos);
}

void main() 
{
    // find current position velocity value 
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
    vec4 Energy = Field(pos);

    // find surrounding velocity values
    vec4 pX = Field(pos + ivec2(1, 0));
    vec4 nX = Field(pos - ivec2(1, 0));
    vec4 pY = Field(pos + ivec2(0, 1));
    vec4 nY = Field(pos - ivec2(0, 1));

    // average out
    Energy.b = (pX.b + pY.b + nX.b + nY.b) * 0.25;

    // find surrounding force
    vec2 Force;
    Force.x = nX.b - pX.b;
    Force.y = nY.b - pY.b;
    Energy.xy += Force * 0.25;

    Energy.b += (nX.x - pX.x + nY.y - pY.y) * 0.25;

    // Gravity force
    // Large division number because of UV to pixel conversions
    // larger the number more viscous the fluid

    Energy.y -= Energy.a / 100000.0;

    Energy.a += (nX.x * nX.a - pX.x * pX.a + nY.y * nY.a - pY.y * pY.a) *0.25;


    int radius = 6;
    if (mousePress && distance(vec2(pos), vec2(mousePos)) < 10.0) {
        Energy.a = 10.0; // inject mass
    }

    // Clamping to avoid energy implosions

    Energy.xy = clamp(Energy.xy, vec2(-1.0), vec2(1.0));

    Energy.b = clamp(Energy.b, 0.0, 1.0);
    Energy.a = clamp(Energy.a, 0.0, 2.0);

    // Storing dye and velocity to access later

    imageStore(outVelTex, pos, vec4(Energy)); 
    imageStore(outDyeTex, pos, vec4(0.0,0.0,0.0,Energy.a));
}   