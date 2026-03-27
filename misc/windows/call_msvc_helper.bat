@ECHO off
@REM Calls the MSVC helper tool if it exists. If not, it tells you off and exits.
@REM Requires being ran in administrator mode. If not in admin mode, the tool
@REM will fail because editing the windows registry is considered a priviledged
@REM task.
SETLOCAL

@REM Setup current directory to return to
SET cdir=%~dp0
SET globdir=%cdir:misc\windows\=%

@REM Obtain VCVARS environment prior to running
CALL "%cdir%get_vc_env.bat"
if %ERRORLEVEL% NEQ 0 pause

"%globdir%bin\set_msvc_paths.exe"
IF %ERRORLEVEL% NEQ 0 echo Unable to locate the file, or this file was not run with administrator permissions. Please run this command appropriately.
ENDLOCAL
