REM Build script for testbed
@ECHO OFF
SetLocal EnableDelayedExpansion


REM Get a list of all the .c files.
SET cFilenames=
FOR /R %%f in (*.c) do (
    SET cFilenames=!cFilenames! %%f
)

SET assembly=testbed
SET compilerFlags=-g -Wall -Werror
REM -Wall -Werror
SET includeFlags=-Isrc -I../engine/src/
SET linkerFlags=-L../bin/ -lengine.lib -L../lib/ -lglfw3dll
SET defines=-D_DEBUG -DVIMPORT

ECHO "Building %assembly%%..."
clang %cFilenames% %compilerFlags% -o ../bin/%assembly%.exe %defines% %includeFlags% %linkerFlags%