@ECHO OFF
@REM Build file for the system environment utility script.
@REM Sets up CL, LINK and LLVM (if installed) to be added to the global
@REM system PATH and their respective includes. We do this because of
@REM how make functions, and how that is antithetical to MSVC.
SETLOCAL

@REM Run the VCENV script to get the environment set up properly.
SET cdir=%~dp0
SET globdir=%cdir:misc\windows\=%
CALL "%cdir%get_vc_env.bat"
if %ERRORLEVEL% NEQ 0 pause

@REM Create bin directory if it doesn't exist. Assumes that the build directory is, in fact, bin.
if not exist "%globdir%bin" mkdir "%globdir%bin"

set ARGS=/nologo /Zc:enumTypes /Wall /wd5039 /EHsc
set LIBS=user32.lib advapi32.lib
set OUTFILE=%globdir%/vcutil.exe
cl %ARGS% /Fe:%OUTFILE% utils/windows/set_msvc_paths.cpp %LIBS%
if %ERRORLEVEL% NEQ 0 pause
ENDLOCAL
