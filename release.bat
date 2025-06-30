@echo off
if not exist build-release (
    mkdir build-release
)
cd build-release

cmake -DCMAKE_BUILD_TYPE=Release ^
      -DCMAKE_TOOLCHAIN_FILE=D:/vcpkg/scripts/buildsystems/vcpkg.cmake ^
      ..

cmake --build . --config Release

