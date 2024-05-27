# Victoria Engine
A game engine I (Paperzlel) want to make in C

## Goals
Currently I want to use this engine to learn C, OpenGL and eventually Vulkan. My first few goals are:
1. Make a window with verbose logging
2. Make a triangle
3. Make a basic 2D game
4. Have a basic UI for editing
5. ???

## Building the engine
This engine uses clang and MSVC. You'll need to get clang from llvm.org, and get Visual Studio Build Tools from their website.
For Visual Studio Build Tools, all you actually need is the Windows Universal C Runtime, MSVC v143 for whatever version you use, and the Windows 10 SDK (latest).
The SDK is the most important because it allows you to use standard libraries in the engine with clang, so get that over anything.

## Support
If you have any issues with the engine at all, please open an issue on GitHub to let me know so I can look at it. 