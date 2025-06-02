#version 430 core
in vec2 uv;
out vec4 color;
uniform sampler2D tex;
void main() {
    float data = texture(tex, uv).r;
    color = vec4(data,data,data,data);
    }