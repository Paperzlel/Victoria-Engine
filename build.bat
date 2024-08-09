@ECHO off
REM Build script for the Victoria Engine executable
SetLocal EnableDelayedExpansion

REM Gets all the C++ source files in the project
SET cppFiles=
for /R %%f in (*.cpp) do (
    SET cppFiles=!cppFiles! %%f
)

REM Gets all C source files in the project
SET cFiles=
for /R %%f in (*.c) do (
    SET cFiles=!cFiles! %%f
)

SET cwd=%cd%

SET compilerFlags=-g -Wall -Werror
SET includeFlags=-I%cwd%
SET linkerFlags=-luser32 -lopengl32
SET defines=-DDEBUG

ECHO "Building the executable..."
clang++ %cppFiles% -x c++ %cFiles% %compilerFlags% -o bin/victoria.exe %defines% %includeFlags% %linkerFlags%
