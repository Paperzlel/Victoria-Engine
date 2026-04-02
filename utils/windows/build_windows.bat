@REM Call make from the command line to use cl instead of clang.
call "%~dp0build_windows.bat"
cd ../../
make %*
