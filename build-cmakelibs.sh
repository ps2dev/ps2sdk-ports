#!/bin/bash

## Determine the maximum number of processes that Make can work with.
## Also make preparations for different toolchains
PROC_NR=$(getconf _NPROCESSORS_ONLN)
XTRA_OPTS=""
MAKECMD=make
OSVER=$(uname)
if [ ${OSVER:0:5} == MINGW ]; then
  XTRA_OPTS=(. -G"MinGW Makefiles")
  MAKECMD=${OSVER:0:7}-make
else
  XTRA_OPTS=(. -G"Unix Makefiles")
fi

CMAKE_OPTIONS="-Wno-dev -DCMAKE_TOOLCHAIN_FILE=$PS2SDK/ps2dev.cmake -DCMAKE_INSTALL_PREFIX=$PS2SDK/ports -DBUILD_SHARED_LIBS=OFF "
#CMAKE_OPTIONS+="-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON "

function build {
    cd $1
    mkdir -p build
    cd build
    cmake $CMAKE_OPTIONS $2 "${XTRA_OPTS[@]}" .. || { exit 1; }
    ${MAKECMD} --quiet -j $PROC_NR clean all install || { exit 1; }
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
git clone --depth 1 -b VER-2-10-4 https://github.com/freetype/freetype || { exit 1; }
git clone --depth 1 -b 0.2.5 https://github.com/yaml/libyaml || { exit 1; }
git clone --depth 1 -b 2.1.0 https://github.com/libjpeg-turbo/libjpeg-turbo || { exit 1; }
git clone --depth 1 -b v1.3.5 https://github.com/xiph/ogg.git || { exit 1; }
git clone --depth 1 -b v1.3.7 https://github.com/xiph/vorbis.git || { exit 1; }
# We need to clone the whole repo and point to the specific hash for now, 
# till they release a new version with cmake compatibility
git clone https://github.com/xiph/opusfile.git || { exit 1; } 
(cd opusfile && git checkout cf218fb54929a1f54e30e2cb208a22d08b08c889 && cd -) || { exit 1; }

##
## Build cmake projects
##
PROC_NR=1 build zlib "-DUNIX:BOOL=ON" # Forcing to compile with -j1 because there is a race condition in zlib
build libpng "-DPNG_SHARED=OFF -DPNG_STATIC=ON"
build freetype
build libyaml
build libjpeg-turbo "-DCMAKE_BUILD_TYPE=Release -DENABLE_SHARED=FALSE -DWITH_SIMD=0"
build ogg
build vorbis
build opusfile "-DOP_DISABLE_HTTP=ON -DOP_DISABLE_DOCS=ON -DOP_DISABLE_EXAMPLES=ON"
cd ..
