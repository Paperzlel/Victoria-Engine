#pragma once

#include "defines.h"
#include "core/logger.h"

#include "renderer-types.inl"


b8 FrontendInitializeRendering(string applicationName);

i8 FrontendRenderFrame();

void FrontendShutdownRendering();

f32 GetDeltaTime();
