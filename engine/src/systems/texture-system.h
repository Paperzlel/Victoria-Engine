#pragma once

#include "defines.h"

u8* LoadTextureFromDisk(string filepath, i32* width, i32* height, i32* channels);

void FreeTextureFromMemory(u8* data);

