# CHANGELOG
Each changelog will have a version number and its progress marker number (e.g. 01, 02, and so on) listed.


## 0.0.7 (06):
### Additions:
- Engine:
    - Containers:
        - Added the `array.c` and `array.h` files.
            - Used for uniform containing, and is scaffolding to later features.
    - Systems:
        - Added `file-system.c` and `file-system.h`.
            - Used to make file handling possible for more systems, at this point in time just our shaders. Images are loaded by `stbi_image` which uses file handling regardless.
## Modifications:
- Assets:
    - Changed `test_shader.fs` to use both the colour range and the texture coordinates.
    - Changed `test_shader.vs` to use a transform coordinate to determine position.
        - Both of these changes are part of me testing the newer systems, they're likely to change frequently and once a better resource management system is in place these will be gone
- Engine:
    - Core:
        - Added memory usage logging to `application.c`.
            - Addition from a previously unused function that was not implemented after we changed from direct render calls to the application layer.
        - Removed include from `application.h`.
            - Pointless include as we only need it in the C file rather than here.
        - Removed the `MemoryTagTypes` parameter from `VZero`.
            - Change to reflect that zeroing out memory was not always removing the memory from the platform and that the memory was still in use.
        - Added method `VReallocate` to `vmemory.c`.
            - Allows us to move memory around rather than implicitly copy it everywhere.
        - Added method `StringSplit` to `vstring.c`.
            - Allows us to have a string taken down into its constituent parts based on a delimiter, those being held in a new `StringArray` type.
    - Maths:
        - Added a `vec4` type to `maths-types.h`.
            - New type which will be used for multiple operations.
        - Added operations for `vec2`, `vec3`, `vec4`, and `mat4` to `vmaths.h`.
            - Needed for later when doing the more complex rendering/world space/camera systems.
        - Added more general maths functions to `vmaths.c`.
            - Needed in some of the maths operations, as well as being generally useful for users.
    - Renderer:
        - OpenGL:
            - Added some transformation handling to `opengl-backend.c`.
                - Temporary, but useful for testing out the new matrix operations.
            - Changed `opengl-shaders.c` to use the new filesystem instead of using standard libraries
                - Mainly to reduce the number of standard library functions accessible to the rest of the engine.
            - Added the new methods `GLListAllUniforms` and `GLUseUniform` to `opengl-shaders.c`.
                - Partial completion of the new uniform features. Currently not fully usable as we need a way to pre-define a uniform's behaviour within the application without allowing open access to the renderer.
    - Systems:
        - Removed the two structs listed in `geometry-system.h`.
            - These files aren't yet used and so we're going to ignore them for now.


## 0.0.6 (05):
### Additions:
- Assets:
    - Added `container.jpg`.
        - Test texture, not staying around for long.
- Engine:
    - Maths:
        - Added `maths-types.h`.
            - Header file for all the main maths objects, like vectors, matrices, quaternions, and so forth.
    - Renderer:
        - Added `renderer-backend.c` and `renderer-backend.h`.
            - New files to handle a more renderer-agnostic way of running functions.
        - OpenGL:
            - Added `opengl-textures.c` and `opengl-textures.h`.
                - Handling texture creation, assignment and destruction.
            - Re-added `glad.c`.
                - Part of the replacement of these files so the user doesn't have to get these when building for the first time.
    - Systems:
        - Added `geometry-system.h`, `texture-system.c` and `texture-system.h`.
            - Not fully in use yet and require more robust systems, but here as scaffolding for later features.
        - This is a new folder for systems that are neither core to the application nor renderer-specific, so they will reside here.
    - Third-Party:
        - Added `stb_image.h`.
            - Handling image loading.
        - This is a new folder for files that are not mine in any way but must be compiled with the engine in order to function.
- Include:
    - Added back the GLAD/GLFW source files.
        - I changed my mind on whether to leave these out and decided it's worth keeping the source files but not the .lib/.dll files, those will have to be included manually. Updated the README on how to obtain these.
### Modifications:
- .vscode:
    - Auotmated file association changes in `settings.json`.
- Assets:
    - Changed `test_shader.fs` and `test_shader.vs` to use texture sampling instead of colour coordinates.
        - Testing new systems. Will change frequently, feel free to ignore.
- Engine:
    - Updated `engine.md`.
        - Documentation update, mainly for contributing/forking.
    - Added new definitions to `defines.h`.
        - Added `VINLINE` and `VNOINLINE` to allow for some code that would flag as a compiler error to work regardless.
    - Edited `entry.h` to use updated functions and remove the while loop from the entrypoint.
        - Application now loops on the application layer so that less systems need to be defined with `VAPI`.
    - Core:
        - Additional functionality to `application.c`.
            - The application now loads here instead of the renderer, allowing for lower-level code to remain low-level and reducing user access to renderer code (this is a good thing).
    - Maths:
        - Added vec2 methods to `vmaths.c`.
            - Scaffolding for later. Not currently significant.
    - Renderer:
        - Changed code to use the new backend calls in `renderer-frontend.c`.
            - Backend is now much cleaner and simpler to use. 
        - Removed the old `RendererObject` in favour of `RendererBackend` in `renderer-types.inl`.
            - Cleaner code, as well as the ability to use function pointers to make renderer-agnostic code.
        - Modified rendering a frame to use an i8 instead of a b8.
            - This is to allow us to shutdown the application without referencing application-layer code in the renderer.
        - OpenGL:
            - Added texture logic to `opengl-backend.c`, as well as a static context.
                - The former is more functionality (although much will change in the future), the latter is a change to reflect the "burying" of lower-level code in the engine rather than constantly passing context in the application layer.
            - Added buffer deletion to `opengl-buffers.c`.
                - Moving code to our definitions instead of GLAD's.
            - Fixed a memory error when freeing memory during file reading in `opengl-shaders.c`.
                - This bug is an indication of a lack of memory knowledge on my part.
            - Moved around struct ordering in `opengl-types.inl`.
                - Code change to remove the former `RenderObject` which has now been absorbed into `OpenGLContext`.
- General:
    - Updated `.gitignore` to remove the `include` and `glad.c` ignores.
        - Part of the revert to including GLFW and GLAD with the engine.


## 0.0.5 (04):
### Additions:
- Assets:
    - Added `test_shader.fs` and `test_shader.vs`.
        - Renaming of assets. Nothing major to add.
- Engine:
    - Core:
        - Added `vmemory.c` and `vmemory.h`.
            - Memory management components, will be expanded upon when needed.
        - Added `vstring.c` and `vstring.h`.
            - String methods. Will also be expanded upon for greater user when needed.
    - Maths:
        - Added `maths` folder, `vmaths.c` and `vmaths.h`.
            - Maths-related methods. Will be expanded upon for more methods and types in the future.
        
    - Renderer:
        - OpenGL:
            - Added `opengl-arrays.c`, `opengl-arrays.h`, `opengl-buffers.c`, `opengl-buffers.h` and `opengl-types.inl`.
                - Created for a more streamlined approach to loading of OpenGL components rather than calling directly from code.
        - Added `renderer-types.inl`.
            - Consolidating structs to one place to avoid messy dependencies.
### Modifications:
- Engine:
    - Core:
        - Added the `ApplicationInitialize` method to `application.c` and application.h`.
            - Functionality changes, part of a movement to call application-layer code from the entrypoint rather than renderer-based code, as more features that require being initialized/shutdown are made.
        - Added documentation to `logger.c` and `logger.h`.
            - Readability changes to code to prevent it becoming more messy.
    - Renderer:
        - OpenGL:
            - Refactored `opengl-backend.c` and `opengl-backend.h` to accomodate for the new files.
                - Mainly for readability and stability, more changes will occur when needed.
            - Refactored `opengl-shaders.c` and `opengl-shaders.h` to accomodate for the new files.
                - New functionality was available from the new files added, so they have been updated to use those methods instead of direct calls.
        - Changed logging outputs in `renderer-frontend.c`.
            - These reflect the changes of where each function is called.
        - Added a new function in `renderer-frontend.h`.
            - `GetDeltaTime` method returns the time between each frame being rendered, as of testing ~60fps.
    - Added new define in `defines.h`.
        - `MAX_LOG_ENTRY` is a cap on how many characters a log entry can have, at 32000 at the moment.
    - `entry.h` now calls application-layer code instead of the renderer directly.
        - Part of a previously mentioned shift in the way we call code. More edits to this file are likely to reflect this change.
- Testbed:
    - Removed newline character in `entry.c`.
        - Cleanup. Will probably exist in the future.
### Removals:
- Assets:
    - Removed `test_shader.frag.glsl` and `test_shader.vert.glsl`.
        - Renaming change to reflect how we now load shaders.


## 0.0.4.1 (03):
### Modifications:
- General:
    - Updated `CHANGELOG.md`.
        - Fixed formatting of the changelog not being how it was intended to look.
        - Meant to be a quick release rather than an official version but it's worth noting regardless.
        - Future updates to the changelog will be omitted from itself.


## 0.0.4 (03):
### Additions:
- Assets:
    - Added `test_shader.frag.glsl` and `test_shader.vert.glsl`
        - Test assets. Unlikely to stay around for long until proper solutions are found.
- Engine:
    - Renderer:
        - OpenGL:
            - Added `opengl-shaders.c` and `opengl-shaders.h`
                - Files that handle shader getting and loading from disk
- General:
    - Added `CHANGELOG.md`
        - This file! Designed so one can keep track of all the changes version-by-version, and so one can check all the progress for every commit.
### Modifications:
- .vscode:
    - Updated `includePath` variable in `c_cpp_properties.json`.
        - Includes third-party header files for Intellisense. 
    - Automated file association changes in `settings.json`
- Engine:
    - General:
        - Modified `build.bat`.
            - Now has the include folder where the third-party header files are found as an include flag
        - Modified `entry.h` to remove `VDEBUG` call.
            - Unnecessary logging call.
    - Renderer:
        - OpenGL:
            - `opengl-backend.h`:
                - Added `u32 shaderProgram`, `u32 VAO`, `u32 VBO` and `u32 EBO` to `struct openGLContext`.
                    - Mainly to concentrate information in one place when allocating OpenGL function calls across files.
                - Changed `BackendRenderFrame` and `BackendShutdownRendering` to take in `openGLContext*` instead of `GLFWwindow*`.
                    - Linked to above change of concentrating where rendering data is stored. Will eventually revisit.
                - Changed `#include` flags for GLAD and GLFW to reflect their new position on the filesystem.
                    - Wraps in `<>` instead of `""`.
            - `opengl-backend.c`:
                - Added shader logic to `BackendInitializeGLFW` in `opengl-backend.c`.
                    - Mainly temporary, as code here needs to be called in a more option-specific manner later down the line. Keeping for simplicity's sake.
                - Added more methods to bind and draw calls in `BackendRenderFrame` as well as destruction calls in `BackendShutdownRendering`.
                    - Better resource allocation. Memory calls will need to be implemented at a later date.
        - `renderer-frontend.c`:
            - Changed method passing from `object->context.window` to `&object->context`.
                - Change to reflect the calls in the backend.
- General:
    - Added more info to `README.md`.
        - Better information on how to use clang, MSVC, GLFW and GLAD in a more tutorial-esque manner.
- Testbed:
    - Added include folder to the list of include flags in `build.bat`.
        - Change to reflect other changes in file structure. General file structure from here on out should remain roughly the same.
### Removals:
- Engine:
    - Renderer:
        - OpenGL:
            - Removed `glad.c`, `glad.h`, `glfw3.h`, `glfw3native.h`, `khrplatform.h`, and `LICENSE.md`.
                - These files are now included separately from the engine. See the changes to `README.md` for more information.


## 0.0.3 (02):
### Additions:
- Engine:
    - Core:
        - Added `application.c`, `application.h`, `logger.c` and `logger.h`.
            - These are files that are intended to be used widely or input into widely by other systems (in the case of application.h).
    - General:
        - Added entry.h
            - This is the application's main entry point for the forseeable future.
    - Renderer:
        - OpenGL:
            - Added `LICENSE.md`, `glad.c`, `glad.h`, glfw3.h`, `glfw3native.h`, `khrplatform.h`.
                - NOTE: These are only here for this commit and are removed in the next one where the user must add them manually.
            - Added `opengl-backend.c` and `opengl-backend.h`.
        - Added `renderer-frontend.c` and `renderer-frontend.h`.
            - These are named so in the distant future one can use both OpenGL, DirectX and Vulkan at once, and possibly more.
- Testbed:
    - Added entry.c
        - Re-naming of `main.c`. Acts as a bridge between the `testbed.exe` and the `engine.dll`.
### Modifications:
- .vscode:
    - Changed `settings.json` to make the internal shell window in Visual Studio Code use Windows PowerShell.
    - Some automated file association changes.
- Engine:
    - General:
        - Modified `build.bat` to include GLFW libraries.
            - These libraries are to be input by the user and not given with the application.
- General:
    - Updated README.md to include information on GLFW and GLAD.
- Testbed:
    - Modified `build.bat` to include GLFW libraries and flag errors with compiling.
        - See above for GLFW libraries. It's unclear why there weren't any error flags on testbed's `build.bat`, but they're now included to be safe.
### Removals:
- Engine:
    - General:
        - Removed `test.c` and `test.h`.
            - These files were temporary for checking that clang was compiling correctly and are no longer needed.
- Testbed:
    - Removed `main.c`.
        - Now renamed to `entry.c`.


## 0.0.2 (01 (listed as #1 in commit history)):
### Additions:
- .vscode:
    - Added `c_cpp_properties.json`, `launch.json`, `settings.json` and `tasks.json`.
- Engine:
    - Added `build.bat` and `engine.md`.
    - Added `defines.h`, `test.c` and `test.h`.
- Testbed:
    - Added `build.bat` and `testbed.md`.
    - Added `main.c`.
### Modifications:
- General:
    - Modified `README.md` to contian more information.

## 0.0.1 (00/initial commit):
- General:
    - Added `LICENSE`, `README.md`, `.gitignore`, and `.gitattributes`.