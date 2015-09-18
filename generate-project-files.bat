@ECHO OFF
rmdir /S /Q build
mkdir build
cd build
cmake .. -G "Visual Studio 12 2013"
pause
