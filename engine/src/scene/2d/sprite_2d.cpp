#include <scene/2d/sprite_2d.h>

void Sprite2D::_get_dest_rect(Vector2 &p_position, Vector2 &p_size) {
    Vector2 offset;
    Vector2 size;
    if (centred) {
        Vector2 centre = texture->get_size() * 0.5;
        offset -= centre;
        size = centre * 2;
    } else {
        size = texture->get_size();
    }

    p_position = offset;
    p_size = size;
}

void Sprite2D::request_redraw() {
    if(texture.is_valid()) {
        Vector2 pos;
        Vector2 size;
        _get_dest_rect(pos, size);

        canvas_set_texture_rect(texture, pos, size);
    }
}

Ref<Texture> Sprite2D::get_texture() const {
    return texture;
}

void Sprite2D::set_texture(const Ref<Texture> &p_texture) {
    if (texture.is_valid()) {
        texture.unref();
    }

    texture = p_texture;
    Vector2 pos;
    Vector2 size;
    _get_dest_rect(pos, size);
    canvas_set_texture_rect(texture, pos, size);
}

bool Sprite2D::is_centred() const {
    return centred;
}

void Sprite2D::set_centred(bool p_value) {
    centred = p_value;
    request_redraw();
}

Sprite2D::Sprite2D() {

}

Sprite2D::~Sprite2D() {

}