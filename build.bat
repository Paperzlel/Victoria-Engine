@ECHO off
REM Build script for the Victoria Engine executable
SetLocal EnableDelayedExpansion

WHERE clang++ >NUL
IF %ERRORLEVEL% NEQ 0 ECHO Clang++ is not supported on your machine. Unable to build properly, aborting. && exit

WHERE make >NUL
IF %ERRORLEVEL% NEQ 0 ECHO Make is not supported on your machine. Please install a valid version of Make in order to compile properly. && exit

make -f Makefile.windows.mak all IS_DEBUG=yes