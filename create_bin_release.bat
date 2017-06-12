@echo off

jam -j3 install
jam -sARCH=win64 -j3 install

cd bin

del *.zip
del *.sha256

rem zip -9 sif2.zip sif2.exe
7z a -tzip -mx=9 sif2.zip sif2.exe
del sif2.exe

ren sif2_x64.exe sif2.exe

rem zip -9 sif2_x64.zip sif2.exe
7z a -tzip -mx=9 sif2_x64.zip sif2.exe
del sif2.exe

sha256sum *.zip > sif2.sha256

copy *.zip ..\pkg-src\embedded\tools\