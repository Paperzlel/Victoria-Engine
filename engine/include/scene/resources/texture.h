#pragma once

#include "core/typedefs.h"
#include "core/io/resource.h"
#include "core/variant/variant_caster.h"

class VAPI Texture : public Resource {
    VREGISTER_CLASS(Texture, Resource)
public:

    enum TextureFormat {
        FORMAT_R,
        FORMAT_RGB,
        FORMAT_RGBA,
    };
protected:

    RID texture;

    int width = 0;
    int height = 0;
    int channel_count = 0;

    TextureFormat format = FORMAT_RGB;
public:

    RID get_texture() const;

    int get_width() const;
    int get_height() const;
    Vector2 get_size() const;
    int get_channel_count() const;
    TextureFormat get_texture_format() const;

    void set_data(int p_width, int p_height, int p_channel_count, TextureFormat p_format);

    Texture();
    ~Texture();
};

VARIANT_CAST_ENUM(Texture::TextureFormat);
