#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D ClickPos;

void main()
{
    vec3 color = texture(ClickPos, TexCoords).rgb;
    FragColor = vec4(color,1.0);
}
