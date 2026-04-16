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

#### Running cl for building
`cl.exe` is Microsoft's native compiler for Windows. It sucks to use, primarily due to the Visual Studio installer not putting it on the command line for any old tool to reference it. In order to use `cl` for compilation, run a new command to call `utils/windows/build_windows.bat` from the command-line which will build Victoria for you. The reason for this is due to `cl` needing certain environment variables set up so it can be called (namely a temporary update to the system PATH) which exists in a specific Visual Studio file that is called by `get_vc_env.bat`.


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
