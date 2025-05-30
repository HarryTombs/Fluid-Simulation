#version 430 core
in vec2 uv;
out vec4 color;
uniform sampler2D tex;
void main() {
    vec4 data = texture(tex, uv);
    color = vec4(data.aaaa);
    }