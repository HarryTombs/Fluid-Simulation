#version 330 core

in vec2 TexCoords;

uniform vec2 mousePos;
uniform sampler2D previousFrame;

layout (location=0)out vec4 outW;


void main()
{
    float oldW = texture(previousFrame, TexCoords).r;

    float dist = distance(TexCoords, mousePos);

    float deltaW;

    if (dist < 0.01)
    {
        deltaW = 1.0; 
    }
    else
    {
        deltaW *= 0.8;
    }
    

    outW = vec4(0,0,0,deltaW);
        
}