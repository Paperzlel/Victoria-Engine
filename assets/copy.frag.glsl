#version 460 core

uniform sampler2D texture_sampler;

in vec2 frag_uv;
out vec4 frag_colour;

void main() {
    frag_colour = texture(texture_sampler, frag_uv);
}