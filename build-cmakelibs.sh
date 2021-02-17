#!/bin/bash

## Determine the maximum number of processes that Make can work with.
PROC_NR=$(getconf _NPROCESSORS_ONLN)

CMAKE_OPTIONS="-Wno-dev -DCMAKE_TOOLCHAIN_FILE=$PS2SDK/ps2dev.cmake -DCMAKE_INSTALL_PREFIX=$PS2SDK/ports -DBUILD_SHARED_LIBS=OFF "
#CMAKE_OPTIONS+="-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON "

function build {
    cd $1
    mkdir build
    cd build
    cmake $CMAKE_OPTIONS $2 .. || { exit 1; }
    make --quiet -j $PROC_NR clean || { exit 1; }
    make --quiet -j $PROC_NR all || { exit 1; }
    make --quiet -j $PROC_NR install || { exit 1; }
    cd ../..
}

## Add ps2dev.cmake
cp ps2dev.cmake $PS2SDK/ || { exit 1; }

##
## Remove build folder
##
rm -rf build
mkdir build
cd build

##
## Clone repos
##
git clone --depth 1 -b v1.2.11 https://github.com/madler/zlib || { exit 1; }
git clone --depth 1 -b v1.6.37 https://github.com/glennrp/libpng || { exit 1; }
git clone --depth 1 -b VER-2-10-2 https://github.com/freetype/freetype2 || { exit 1; }
git clone --depth 1 -b 0.2.5 https://github.com/yaml/libyaml || { exit 1; }

##
## Build cmake projects
##
build zlib
build libpng "-DPNG_SHARED=OFF -DPNG_STATIC=ON"
build freetype2
build libyaml
cd ..

##
## Fix legacy applications using libz.a instead of libzlib.a
##
ln -sf "$PS2SDK/ports/lib/libzlib.a" "$PS2SDK/ports/lib/libz.a"
