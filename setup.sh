#!/bin/bash

rm build/CMakeCache.txt

mkdir deps
cd deps

echo "Installing FreeImage..."
wget http://downloads.sourceforge.net/freeimage/FreeImage3180.zip
unzip FreeImage3180.zip
cd FreeImage
make -j8
cd ..

echo "Installing glew..."
wget https://sourceforge.net/projects/glew/files/glew/2.1.0/glew-2.1.0.zip
unzip glew-2.1.0.zip
cd glew-2.1.0
make -j8
cd ..

echo "Installing SDL2..."
wget https://www.libsdl.org/release/SDL2-2.0.8.tar.gz
tar -xvf SDL2-2.0.8.tar.gz
mkdir SDL2-2.0.8/build
cd SDL2-2.0.8/build
../configure
make -j8
cd ../..

cd external/assimp
rm -rf distrib
rm -rf build
mkdir distrib
mkdir distrib/unix
mkdir build
mkdir build/unix
cd build/unix
cmake -G "Unix Makefiles" ../../
make -j8
cp code/libassimp.so* ../../distrib/unix/

echo "Done!"