@echo off

set switches=-Zi -nologo 
set linker_flags=/link user32.lib gdi32.lib winmm.lib /incremental:no
set dll_linker_flags=/link /DLL /EXPORT:game_update_and_render /EXPORT:initialize_window_state /PDB:vertexbyte_%random%.pdb  /incremental:no

cd build
del *.pdb
cl %switches% ../vertexbyte.cpp /Fe"vertexbyte.dll"  %dll_linker_flags% 
cl %switches% ../vertexbyte_win32.cpp /Fe"vertexbyte_win32.exe" %linker_flags% 
cd ..
