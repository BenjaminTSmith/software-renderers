#version 330 core
out vec4 out_color;

in vec2 out_tex;

uniform sampler2D in_texture;

void main() {
    out_color = texture(in_texture, out_tex);
}
