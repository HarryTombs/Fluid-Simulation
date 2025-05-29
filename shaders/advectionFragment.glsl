#version 330 core

in vec2 TexCoords;

uniform vec2 mousePos;
uniform sampler2D previousFrame;

out vec4 FragColor;

layout (location=0)out vec4 outW;

vec4 Field (vec2 position) {
    vec2 velocityGuess = texture(previousFrame, position/ TexCoords).xy;
    vec2 back = position - velocityGuess;
    return texture (previousFrame, back);
}


void main()
{
    float oldW = texture(previousFrame, TexCoords).r;
    vec2 Me = TexCoords.xy;
    vec4 Energy = Field(Me);

    vec4 pX = Field(Me + vec2(1,0));
    vec4 nX = Field(Me - vec2(1,0));
    vec4 pY = Field(Me + vec2(0,1));
    vec4 nY = Field(Me - vec2(0,1));

    Energy.b = (pX.b + nX.b + pY.b, nY.b) / 4.0;
    vec2 force; 
    force.x = nX.b - pX.b;
    force.y = nY.b - pX.b;
    Energy.xy += force / 4.0;
    Energy.b += (nX.x - pX.x + nY.y - pY.y) / 4.0;
    Energy.y -= Energy.w /30000.0;
    Energy.w += (nX.x * nX.w - pX.x * pX.w + nY.y * nY.w - pY.y * pY.w) / 4.0;


    if(Me.x < 1.0 || Me.y < 1.0 || TexCoords.x - Me.x < 1.0 || TexCoords.y - Me.y < 1.0) {
        Energy.xy = vec2(0.0);
    }

    float dist = distance(TexCoords, mousePos);

    float deltaW;

    if (dist < 0.01)
    {
        Energy.w = 1.0;
    }
    else
    {
        Energy.w *= 0.0;
    }
    
    FragColor = Energy;

    outW = vec4(0,0,0,Energy.w);
        
}