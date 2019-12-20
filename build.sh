#!/bin/bash

python process_texture.py

BUILD_TYPE=$1

cd $PWD
if [ ! -d build ]; then
  mkdir build
fi
cd build

DEPS_PATH=/scr/junwon/skyline/cg/deps

cmake ../ -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DGLEW_INCLUDE_DIR=$DEPS_PATH/glew-2.1.0/include/ \
    -DGLEW_LIBRARY=$DEPS_PATH/glew-2.1.0/lib/libGLEW.a \
    -DFREEIMAGE_INCLUDE_DIRS=$DEPS_PATH/FreeImage/Source/ \
    -DFREEIMAGE_LIBRARY=$DEPS_PATH/FreeImage/Dist/libfreeimage.a \
    -DSDL2_INCLUDE_DIR=$DEPS_PATH/SDL2-2.0.8/include/ \
    -DSDL2_LIBRARY=$DEPS_PATH/SDL2-2.0.8/build/build/.libs/libSDL2.so

make -j8

./cs148raytracer