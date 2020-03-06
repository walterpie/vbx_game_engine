@echo off


cd build
cl -Zi -nologo ../vertexbyte_win32.cpp /Fe"vertexbyte_win32.exe" /link user32.lib gdi32.lib winmm.lib
cd ..
