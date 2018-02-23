@echo off

set CommonCompilerFlags -nologo -O2 -fp:fast -Wall

IF NOT EXIST ..\..\build mkdir ..\..\build
pushd ..\..\build

cl %CommonCompilerFlags% ..\code\chapter3\chapter3.cpp

popd