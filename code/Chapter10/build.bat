@echo off

set CommonCompilerFlags=-nologo -O2 -fp:fast

IF NOT EXIST ..\..\build mkdir ..\..\build
pushd ..\..\build

cl %CommonCompilerFlags% ..\code\chapter10\chapter10.cpp

popd