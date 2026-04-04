# Thirdparty Libraries
Here is a list of all third-party libraries and their corresponding licenses and modifications.

## angle
- Source: https://chromium.googlesource.com/angle/angle
- Version: Git commit 780bcd7b1195521251ba61e8b5a7760650ec2505
- License: MIT
Modifications:
- Extracted `include/EGL/*` and `include/KHR/*` to own source tree
- Removed `.clang-format` files found in the folders.

## freetype
- Source: https://gitlab.freedesktop.org/freetype/freetype
- Version: 2.13.3 (2024)
- License: FreeType License
Modifications:
- Removed all of the folders aside from `src/` and `include/`
- Removed the `dlg` folder from both `src/` and `include/`
- Included `LICENSE.TXT` and `docs/FTL.TXT` for license information
- Moved `include/` to our engine's `include/` directory to maintain consistency

## GLAD
- Source: https://github.com/Dav1dde/glad
- Version: 2.0.8
- License: MIT, Apache 2.0
Modifications:
- Moved the `glad/` include folder to our own maintained include folder for consistency.
- Added `LICENSE.txt`

## NVAPI
- Source: https://download.nvidia.com/XFree86/nvapi-open-source-sdk/
- Version: R525
- License: MIT
Modifications:
- Removed unecessary code from `nvapi.h` and renamed the file.

## STB
- Source: https://github.com/nothings/stb
- Version: N/A
- License: MIT
Modifications:
- Cherry-picked `stb_image.h`
- Added `stb_image.cpp` for an implementation source
- Moved `stb_image.h` to the include directory with all other files for consistency.
- Included `LICENSE.txt`

## Wayland
- Source: https://gitlab.freedesktop.org/wayland/wayland
- Version: N/A
- License: Expat
Modifications:
- Extracted `protocol/wayland.xml` from source.

## wayland-protocols
- Source: https://gitlab.freedesktop.org/wayland/wayland-protocols
- Version: N/A
- License: Expat
Modifications:
- Extracted `stable` `staging` and `unstable` and the subsequent XML files from source.
