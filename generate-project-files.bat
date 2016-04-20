@ECHO OFF
rmdir /S /Q build
mkdir build
cd build
cmake .. -G "Visual Studio 14 2015" -DSDL2DIR=%THIRD_PARTY_DIR%/SDL2
pause
