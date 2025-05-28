#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D outW;

void main()
{
    float W = 0;
    W = texture(outW,TexCoords).a;
    FragColor = vec4(W,W,W,W);
}
