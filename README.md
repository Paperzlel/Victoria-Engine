# Victoria Engine
A game engine I (Paperzlel) want to make in C

## Goals
Currently I want to use this engine to learn C, OpenGL and eventually Vulkan. My first few goals are:
1. Make a window with verbose logging (DONE)
2. Make a triangle (DONE)
3. Make a basic 2D game
4. Have a basic UI for editing
5. ???

## Building the engine
This engine uses clang and MSVC, as well as GLFW and GLAD for OpenGL compiling. You'll need to get clang from https://llvm.org/builds/, and get Visual Studio Build Tools from https://visualstudio.microsoft.com/downloads/. GLFW can be found at https://www.glfw.org/download, and GLAD is included within the project itself, so don't worry about downloading it.

Clang only requires you set up a system PATH variable so that it can be accessed by the batchfiles. Aside from that there's not a lot you need to do here.

For Visual Studio, you only want the Build Tools (I use VS Code which is also available at that link), and specifically the Windows 10 SDK for you standard C libraries which we need for certain parts of the application. With the current setup I have I would not reccommend building with Visual Studio itself, as you would need to set up the linkers yourself and building will be a whole other hassle, so only do so if you really want to.

For GLFW, you'll need to download the source, and find the lib-static-ucrt folder where the .lib and .dll files are. Make a folder called lib in the top of the repository and place your glfw3dll.lib file in that folder. If it's not already there, make a bin file in the same directory and place the glfw3.dll in the top of the bin folder. 

## Support
If you have any issues with the engine at all, please open an issue on GitHub to let me know so I can look at it. 