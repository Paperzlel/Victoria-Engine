# Thirdparty Libraries
Here is a list of all third-party libraries and their corresponding licenses and modifications.

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
- Removed code from `nvapi.h` and renamed the file.


## STB
- Source: https://github.com/nothings/stb
- Version: N/A
- License: MIT
Modifications:
- Cherry-picked `stb_image.h`
- Added `stb_image.cpp` for an implementation source
- Moved `stb_image.h` to the include directory with all other files for consistency.
- Included `LICENSE.txt`
