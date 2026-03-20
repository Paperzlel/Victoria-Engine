#include "scene/resources/texture.h"

#include "servers/rendering_server.h"

RID Texture::get_texture() const {
    return texture;
}

int Texture::get_width() const {
    return width;
}

int Texture::get_height() const {
    return height;
}

Vector2 Texture::get_size() const {
    return Vector2(width, height);
}

int Texture::get_channel_count() const {
    return channel_count;
}

Texture::TextureFormat Texture::get_texture_format() const {
    return format;
}

void Texture::set_data(int p_width, int p_height, int p_channel_count, TextureFormat p_format) {
    width = p_width;
    height = p_height;
    channel_count = p_channel_count;
    format = p_format;
}

Texture::Texture() {
    texture = RS::get_singleton()->texture_allocate();
}

Texture::~Texture() {
    if (texture.is_valid()) {
        RS::get_singleton()->texture_free(texture);
    }
}