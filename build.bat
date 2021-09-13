@echo off

if not exist .\build mkdir .\build

pushd .\build
cl -nologo ..\tlib_win32.c -Zi -FC kernel32.lib user32.lib gdi32.lib
popd
