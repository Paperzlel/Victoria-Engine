@REM Setup current directory to return to
SET cdir=%~dp0
SET globdir=%cdir:misc\windows\=%

@REM Find the installation directory name for Visual Studio
:begin
SET __VC_DIR=C:\Program Files\Microsoft Visual Studio
if exist "%__VC_DIR%" (
	goto find-vs-version
)

SET __VC_DIR=C:\Program Files (x86)\Microsoft Visual Studio
if exist "%__VC_DIR%" (
	goto find-vs-version
)

echo Unable to find a valid Visual Studio installation folder.
exit /B 0

@REM Find the version of Visual Studio installed. This is preferred to be the latest one.
:find-vs-version
SET __VC_INSTALL_DIR_ITEMS=
cd %__VC_DIR%
for /D %%f in (*) do (
	if not defined __VC_INSTALL_DIR_ITEMS (
		SET __VC_INSTALL_DIR_ITEMS=%%f
	)
)
cd %globdir%

@REM Create full directory string
SET __FULL_VC_ROOT=%__VC_DIR%\%__VC_INSTALL_DIR_ITEMS%

@REM Triple-check that the directory has contents. If not, throw an error (we can't
@REM delete it ourselves). Requires a workaround since our path CAN be "Program Files (x86)"
@REM which for SOME reason beats the batchfile sanitation and makes the parenthesis
@REM close prematurely.
dir /b /s /a "%__FULL_VC_ROOT%" | findstr .>nul || goto print-dir-error

goto open-vs-prompt
:print-dir-error
echo Unable to use directory '%__FULL_VC_ROOT%' as it existed but contained no data, likely from an old Visual Studio installation. Please delete the directory and try again.
pause
exit /B 1

:open-vs-prompt
@REM VCVARS64 can be installed in the Visual Studio Community directory or the Visual
@REM Studio Build Tools directory. Check VS community first, then VS Build Tools.
@REM If other options exist, add them to the setup. This logic may wind up in another
@REM batchfile should it get too long.
SET __FOUND_VC=0

call "%__FULL_VC_ROOT%\Community\VC\Auxiliary\Build\vcvars64.bat" 2>NUL
if %ERRORLEVEL%==0 set __FOUND_VC=1

if %__FOUND_VC%==0 (
	call "%__FULL_VC_ROOT%\BuildTools\VC\Auxiliary\Build\vcvars64.bat" 2>NUL
	if %ERRORLEVEL%==0 SET __FOUND_VC=1
)

if __FOUND_VC==0 (
	echo Unable to locate a valid Visual Studio VCVARS script. Please locate your Visual Studio installation and open a pull request to get this updated.
)
