#version 330 core

uniform sampler2D iChannel0;
uniform vec2 iResolution;

void main()
{
    vec2 coord = gl_FragCoord.xy;
    vec4 data = texture(iChannel0, coord / iResolution);
    gl_FragColor = vec4(data.w);
}
