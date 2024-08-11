#!/bin/bash
set echo on

mkdir -p bin

cppFiles=$(find . -type f -name "*.cpp")

cFiles=$(find . -type f -name "*.c")

cwd="$PWD"

compilerFlags="-g -fPIC"
includeFlags="-I$cwd"
linkerFlags="-lxcb -lX11 -lX11-xcb -lxkbcommon -L/usr/X11R6/lib -Wl,-rpath,."
defines="-DDEBUG"

echo "Building the executable..."
clang++ $cppFiles -x c++ $cFiles $compilerFlags -o bin/victoria $defines $includeFlags $linkerFlags
