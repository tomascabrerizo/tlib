@echo off

if not exist .\build mkdir .\build

pushd .\build
cl -nologo ..\tlib_build.cpp -Zi -FC
popd
