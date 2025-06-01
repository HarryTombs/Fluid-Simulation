#version 430
layout(local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 0) uniform readonly image2D inputTex;
layout(rgba32f, binding = 1) uniform writeonly image2D outputTex;

uniform ivec2 Resolution;
uniform ivec2 mousePos; 
uniform bool mousePress;

void main() {
    ivec2 pos = ivec2(gl_GlobalInvocationID.xy);

    vec4 color = imageLoad(inputTex, pos);


    if (mousePress) 
    {
        vec2 mouseUV = mousePos / vec2(Resolution);
        vec2 uv = vec2(pos) / vec2(Resolution);
        float dist = length(uv - mouseUV);
        if (dist < 0.05) 
        {
            color.xy += normalize(uv - mouseUV) * 0.5;
            color.zw += vec2(0.0, 0.1);
        }
    }
    imageStore(outputTex, pos, color);
}