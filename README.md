# Victoria-Engine-Dev
 In-development version of Victoria Engine

## Compiling

Python is required in some form on all PCs for the build process, but it should be installed by default on most Linux distributions and exist somewhere on most Windows OSes. `make` and some form of compiler (we only support GCC and clang for the moment) are also required. Specific compilers can be specified with the `COMPILER=` option when compiling, and overriding `CC=` and `CXX=` can set separate compilers for C and C++ compilation, however this is not supported and should be avoided.

### Windows

#### Required
- [Make for Windows](https://sourceforge.net/projects/gnuwin32/) as our buildsystem
- [Visual Studio Build Tools](https://visualstudio.microsoft.com/downloads/?q=build+tools)
	- Option is under "All Downloads" -> "Tools for Visual Studio" (they don't offer it though regular Visual Studio in the 2026 edition)
	- Select `Windows Universal C Runtime`, `MSVC Build Tools for x64/x86 (Latest)` and the `Windows 11 SDK (latest version)` from the "Individual Components" tab to obtains the strictly necessary packages for development.
- [Python](https://www.python.org/downloads/) for build scripts that are run during compilation.
- Some form of compiler. Currently, `clang` and `cl` are supported on Windows

#### Optional
- [MSVC](https://visualstudio.microsoft.com/downloads/?q=build+tools) from the additional options (`C++/CLI support (Latest MSVC)`)
	- See below for how you get the commands into your system `PATH`.
- [Clang](https://github.com/llvm/llvm-project/releases) for compiler services as well as clangd and codeLLDB.
	- These may be automatically installed by development tools in your editor of choice.
- (Optional) [Git for Windows](https://git-scm.com/install/windows) if you wish to contribute to the engine.

If you are running an older version of Windows and lack native OpenGL support (or if your driver is known to be buggy) you may wish to retrieve ANGLE binaries from our own repository. Windows builds will always come with these binaries as they are needed for certain GPUs.

#### Getting cl on the system PATH
**DISCLAIMER: THIS IS NOT THE RECOMMENDED WAY TO DO THIS BY MICROSOFT. IT MAY WELL MESS UP OTHER BUILDSYSTEMS AND VISUAL STUDIO.**

For some reason, Windows doesn't set up the system path properly when you install all their compiler options, which is a massive pain for anybody who doesn't want to use Visual Studio. To work around this, we have kindly provided a build script that detects a Visual Studio installation if it exists, and builds an executable to run, which sets up the development environment. To run this:
1. Navigate to the `misc/windows/` directory.
2. Run the `build_msvc_helper.bat` file. This should produce a file named `vcutils.exe` in the `bin` folder.
3. Run the `call_msvc_helper.bat` file in ADMINISTRATOR mode. One would prefer not to do this, as it's obviously quite dangerous to do so on random stuff, but the source is in `utils/windows/set_msvc_paths.cpp` if you're wondering what it does and (hopefully) it is safe to run.

This will add all the respective commands (`cl`, `link`) to the system PATH, create the `INCLUDE` variable (the paths to the Visual Studio headers and Windows CRT header) and the `LIB` variable (the path to the Visual Studio libraries and Windows CRT libraries). It will also add the LLVM binaries to the path, if found.

We require the batchfiles as we're calling to implement the Visual Studio environment first (with all its variables) before making said variables permanent via the respective options.

**WARNING**: This tool is limited in use - it does not individually detect the existence of each variable and add it to the path if not present. A more conclusive algorithm for this ought to be made which can upgrade detected installations and replace variables individually, but that is far beyond the scope of an indie game engine and will be a project for another time. 


### Linux

Debian-based
```
sudo apt install libx11-dev libgl1-mesa-dev libstdc++-12-dev
```
Fedora-based
```
sudo dnf install libX11-devel wayland-devel
```

## Debugging
By default, we use `clangd` and `clang-format` for debugging and formatting options. To support `clangd` in particular within our buildsystem, we have specific makefile targets that allow one to build a `compile_commands.json` file from the source tree without installing additional dependencies. Run `make generate_compile_commands` or `make generate_compile_commands DEBUG=yes` to build a compilation database for debugging. Code debuggers may whine about missing symbols without this.
