@echo off

if not exist .\build mkdir .\build

pushd .\build
cl -nologo ..\alpha_blt.cpp -Zi -FC
popd
