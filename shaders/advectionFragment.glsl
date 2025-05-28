#version 330 core

uniform sampler2D iChannel0;
uniform vec2 iResolution;
uniform vec2 mousePos;
uniform float iTime;

vec4 Field(vec2 pos)
{
    vec2 velocity = texture(iChannel0, pos /iResolution).xy;
    vec2 back = pos - velocity;
    return texture(iChannel0, back / iResolution);
}

void main()
{
    vec2 Me = gl_FragCoord.xy;
    vec4 Energy = Field(Me);
    vec4 pX = Field(Me + vec2(1,0));
    vec4 nX = Field(Me - vec2(1,0));
    vec4 pY = Field(Me + vec2(0,1));
    vec4 nY = Field(Me - vec2(0,1));

    Energy.b = (pX.b + nX.b + pY.b + nY.b) / 4.0;
    vec2 force;
    force.x = nX.b - pX.b;
    force.y = nX.b - pX.b;
    Energy.xy += force / 4.0;
    Energy.b += (nX.x - pX.x + nY.y - pY.y) / 4.0;
    Energy.y -= Energy.w / 300.0;
    Energy.w += (nX.x * nX.w - pX.x * pX.w + nY.y * nY.w - pY.y * pY.w) /4.0;

    if(Me.x < 10. || Me.y < 10. || iResolution.x - Me.x < 10. || iResolution.y - Me.y < 10.)
    {
        Energy.xy = vec2(0,0);
    }

    if(length(mousePos.xy - Me)< 10.0)
    {
        Energy.w = 1.0;
    }

    gl_FragColor = Energy;
}