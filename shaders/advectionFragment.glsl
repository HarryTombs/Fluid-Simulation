#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D uVelocity;   // the velocity field
uniform sampler2D uSource;     // the thing we're moving (velocity or dye)
uniform float dt;              // timestep
uniform vec2 texelSize;        // 1.0 / resolution

void main()
{
    vec2 vel = texture(uVelocity, TexCoords).xy;
    vec2 backPos = TexCoords - dt * vel * texelSize;

    FragColor = texture(uSource, backPos);
}