@echo off

set CommonCompilerFlags=-D_CRT_SECURE_NO_WARNINGS=1 -Gm- -MTd -nologo -EHa- -GR- -Od -Oi -WX -W4 -wd4700 -wd4505 -wd4201 -wd4100 -wd4101 -wd4189 -FC -Z7 

IF NOT EXIST build mkdir build
pushd build
cl %CommonCompilerFlags%  ..\code\SICXE_Entry.cpp -FmSICXE_Entry.map
popd