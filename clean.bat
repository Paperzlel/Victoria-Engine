@ECHO off

WHERE make >NUL
IF %ERRORLEVEL% NEQ 0 ECHO Make is not supported on your machine. Please install a valid version of Make in order to clean properly. && exit

make -f Makefile.windows.mak clean