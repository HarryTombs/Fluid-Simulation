#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform vec2 mousePos;

layout (location=0)out vec4 ClickPos;

void main()
{
    float dist = distance(TexCoords, mousePos);
    if (dist < 0.01)
    {
        ClickPos = vec4(1.0,1.0, 1.0, 1.0); 
    }
    else
    {
        ClickPos = vec4(0.05,0.15, 0.2, 1.0); 
    }
        
}