#/bin/bash

cd build_windows

cmake --toolchain $PWD/mingw-w64-x86_64.cmake ..

cd ..

cmake --build build_windows -j 12
