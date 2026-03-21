# Victoria-Engine-Dev
 In-development version of Victoria Engine

## Compiling
Python is required in some form on all PCs for the build process, but it should be installed by default on most Linux distributions and exist somewhere on most Windows OSes. `make` and some form of compiler (we only support GCC and clang for the moment) are also required. Specific compilers can be specified with the `COMPILER=` option when compiling, and overriding `CC=` and `CXX=` can set separate compilers for C and C++ compilation, however this is not supported and should be avoided.

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
