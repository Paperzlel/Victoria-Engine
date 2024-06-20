#pragma once

#include "defines.h"

#include "opengl-types.inl"

typedef enum TextureType {
    TEXTURE_2D = GL_TEXTURE_2D,
    TEXTURE_3D = GL_TEXTURE_3D
} TextureType;

typedef enum TextureWrapType {
    REPEAT = GL_REPEAT,
    MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
    CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
    CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER
} TextureWrapType;

typedef enum TextureFilteringType {
    LINEAR = GL_LINEAR,
    NEAREST = GL_NEAREST,
    NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
    NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
    LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
    LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR
} TextureFilteringType;

b8 GenerateTexture(Texture* t, TextureType type);

void SetTextureWrapParameters(TextureType texType, TextureWrapType wrapType);

void SetMipmapLevelParameters(TextureType texType, TextureFilteringType interpTypeMin, TextureFilteringType interpTypeMag);