@echo off

if not exist .\build mkdir .\build

set disableWarnings=-wd4255 -wd4668 -wd4710 -wd4820 -wd5045

pushd .\build
cl -nologo -Wall ..\tlib_win32.c -Zi -FC kernel32.lib user32.lib gdi32.lib %disableWarnings% 
rem cl -nologo -std:c11 ..\tlib_win32.c -Zi -FC kernel32.lib user32.lib gdi32.lib -wd5105 
rem gcc -std=c99 d:\dev\tlib\tlib_win32.c -lkernel32 -luser32 -lgdi32
popd
