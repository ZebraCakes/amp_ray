@echo off

set CommonCompilerFlags=-nologo -O2 -fp:fast
set CommonCompilerFlags=-DDEBUG=0 %CommonCompilerFlags%

IF NOT EXIST ..\..\build mkdir ..\..\build
pushd ..\..\build

cl %CommonCompilerFlags% ..\code\book2_chapter1\motion.cpp

popd