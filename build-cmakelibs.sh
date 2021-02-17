#!/bin/bash

## Determine the maximum number of processes that Make can work with.
PROC_NR=$(getconf _NPROCESSORS_ONLN)

CMAKE_OPTIONS="-Wno-dev -DCMAKE_TOOLCHAIN_FILE=$PS2SDK/ps2dev.cmake -DCMAKE_INSTALL_PREFIX=$PS2SDK/ports -DBUILD_SHARED_LIBS=OFF "
#CMAKE_OPTIONS+="-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON "

function build {
    mkdir $1
    cd $1
    cmake $CMAKE_OPTIONS $3 ../../$2 || { exit 1; }
    make --quiet -j $PROC_NR clean || { exit 1; }
    make --quiet -j $PROC_NR all || { exit 1; }
    make --quiet -j $PROC_NR install || { exit 1; }
    cd ..
}

## Add ps2dev.cmake
cp ps2dev.cmake $PS2SDK/ || { exit 1; }

##
## Build cmake projects
##
rm -rf build
mkdir build
cd build
build zlib      zlib/src
build libpng    libpng/src "-DPNG_SHARED=OFF -DPNG_STATIC=ON"
build freetype  freetype2/src
build libyaml   libyaml
cd ..

##
## Fix legacy applications using libz.a instead of libzlib.a
##
ln -sf "$PS2SDK/ports/lib/libzlib.a" "$PS2SDK/ports/lib/libz.a"
