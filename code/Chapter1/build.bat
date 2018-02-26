@echo off

set CommonCompilerFlags=-nologo -O2 -fp:fast

IF NOT EXIST ..\..\build mkdir ..\..\build
pushd ..\..\build

cl %CommonCompilerFlags% ..\code\chapter1\chapter1.cpp

popd