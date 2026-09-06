@echo off
echo ==========================================
echo  Building Seesaw Tilt EQ for Windows (VST3)
echo ==========================================

if not exist build mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release

echo.
echo Build complete! VST3 is installed in:
echo C:\Program Files\Common Files\VST3\Seesaw Tilt EQ.vst3
pause
