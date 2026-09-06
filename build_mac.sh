#!/bin/bash
set -e
echo "=========================================="
echo " Building Seesaw Tilt EQ for macOS (AU & VST3)"
echo "=========================================="

mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release -j $(sysctl -n hw.ncpu)

echo ""
echo "Build complete! Plug-ins are installed in:"
echo "AU:   ~/Library/Audio/Plug-Ins/Components/Seesaw Tilt EQ.component"
echo "VST3: ~/Library/Audio/Plug-Ins/VST3/Seesaw Tilt EQ.vst3"
