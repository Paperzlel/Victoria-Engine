#version 460 core

in vec2 frag_uv;
in vec4 colour_interp;
in flat int instance_data_flags;

#define ITEM_USE_ALPHA_ONLY 1 << 0
#define ITEM_USE_SDF 1 << 1
#define ITEM_USE_TEXTURE_SUBCOORDS 1 << 2


layout(location = 0) out vec4 frag_colour;

layout (std140) uniform MaterialData { //ubo:2
    vec4 diffuse;
    vec3 ambient;
    vec3 specular;
    float shininess;
};

uniform sampler2D texture_sampler;

void main() {
    vec4 colour = colour_interp;
    colour *= diffuse;
    vec4 texture_vec = texture(texture_sampler, frag_uv);
    
    // Check for Signed Distance Fields in the texture output. Red MUST be the alpha channel.
    if (bool(instance_data_flags & ITEM_USE_SDF)) {
        if (texture_vec.r < 0.5) {
            discard;
        }
    }

    if (bool(instance_data_flags & ITEM_USE_ALPHA_ONLY)) {
        colour *= vec4(1.0, 1.0, 1.0, texture_vec.r);
    } else {
        colour *= texture_vec;
    }

    frag_colour = colour;
}