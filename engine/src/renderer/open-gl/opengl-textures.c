#include "opengl-textures.h"
#include "systems/texture-system.h"

#include "core/logger.h"

b8 GenerateTexture(Texture* t, TextureType type) {
    if (!t) {
        VERROR("Texture reference is null. Unable to generate a texure.");
        return FALSE;
    }
    glGenTextures(1, &t->texID);

    glBindTexture(type, t->texID);

    //Texture wrapping and filtering parameters
    SetTextureWrapParameters(type, REPEAT);
    SetMipmapLevelParameters(type, LINEAR_MIPMAP_LINEAR, LINEAR);

    //TODO: Add custom mipmap levels, add custom filepath stuff
    t->data = LoadTextureFromDisk("C:/Programs/C-and-C++/Victoria-Engine/assets/container.jpg", &t->texWidth, &t->texHeight, &t->channelCount);
    //TODO: Add range allocations (maybe assume RGBA)
    t->range = RGB;
    if (t->data) {
        glTexImage2D(type, 0, t->range, t->texWidth, t->texHeight, 0, t->range, GL_UNSIGNED_BYTE, t->data);
        glGenerateMipmap(type);
    } else {
        VERROR("Failed to load texture from disk.");
        return FALSE;
    }
    FreeTextureFromMemory(t->data);
    return TRUE;
}

// NOTE: These functions need to be called DURING texture initialization 

void SetTextureWrapParameters(TextureType texType, TextureWrapType wrapType) {
    if (texType == TEXTURE_2D) {
        glTexParameteri(texType, GL_TEXTURE_WRAP_S, wrapType);
        glTexParameteri(texType, GL_TEXTURE_WRAP_T, wrapType);
    }
    // 3D textures one day
}

void SetMipmapLevelParameters(TextureType texType, TextureFilteringType filterTypeMin, TextureFilteringType filterTypeMag) {
    glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, filterTypeMin);
    glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, filterTypeMag);
}