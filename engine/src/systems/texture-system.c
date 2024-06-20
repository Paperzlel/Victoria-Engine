#include "texture-system.h"

#define STB_IMAGE_IMPLEMENTATION
#include "thirdparty/stb_image.h"

u8* LoadTextureFromDisk(string filepath, i32* width, i32* height, i32* channelCount) {
    return stbi_load(filepath, width, height, channelCount, 0);
}

void FreeTextureFromMemory(u8* data) {
    stbi_image_free(data);
}