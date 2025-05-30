#version 430
layout(local_size_x = 1, local_size_y = 1) in;

layout(rgba32f, binding = 0) uniform readonly image2D inputTex;
layout(rgba32f, binding = 1) uniform writeonly image2D outputTex;

uniform ivec2 Resolution;
uniform ivec2 mousePos; 
uniform bool mousePress;

vec4 Field(ivec2 pos) 
{
    ivec2 velocity = imageSize(inputTex);
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

    vec4 up = Field(Me + ivec2(0,1));
    vec4 down = Field(Me - ivec2(0,1));
    vec4 left = Field(Me - ivec2(1,0));
    vec4 right = Field(Me + ivec2(1,0));

    Energy.b = (up.b + down.b + left.b + right.b) / 4.0;

    vec2 Force;
    Force.x = left.b - right.b;
    Force.y = down.b - up.b;
    Energy.xy += Force / 4.0;

    Energy.b += (left.x - right.x + down.y - up.y)/4.0;

    Energy.y -= Energy.a/300.0;

    Energy.a += (left.x * left.a - right.x * right.a + down.y * down.a - up.y * up.a)/4.0;

    if (Me.x < 10 || Me.y < 10 || Resolution.x - Me.x < 10 || Resolution.y - Me.y < 10) {
        Energy.xy = vec2(0.0);
    }



    int radius = 6;
    if (mousePress && distance(vec2(Me), vec2(mousePos)) < 10.0) {
        Energy.a = 1.0; // inject mass
    }

    imageStore(outputTex, Me, Energy); 
}   