#version 330 core
layout (location = 0) in vec2 in_pos;
layout (location = 1) in vec2 in_tex;

out vec2 out_tex;

void main() {
    gl_Position = vec4(in_pos, 0, 1);
    out_tex = in_tex;
}
