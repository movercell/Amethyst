#/bin/bash

mkdir build
cd build

CC=gcc CXX=g++ cmake ..

cd ..

cmake --build build -j 12
