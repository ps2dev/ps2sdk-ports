#!/bin/bash

## Determine the maximum number of processes that Make can work with.
## Also make preparations for different toolchains
PROC_NR=$(getconf _NPROCESSORS_ONLN)
CFLAGS=""
XTRA_OPTS=""
MAKECMD=make
OSVER=$(uname)
if [ ${OSVER:0:5} == MINGW ]; then
  XTRA_OPTS=(. -G"MinGW Makefiles")
  MAKECMD=${OSVER:0:7}-make
else
  XTRA_OPTS=(. -G"Unix Makefiles")
fi

CMAKE_OPTIONS=(-Wno-dev "-DCMAKE_TOOLCHAIN_FILE=$PS2DEV/share/ps2dev.cmake" "-DCMAKE_INSTALL_PREFIX=$PS2SDK/ports" -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=RelWithDebInfo "-DCMAKE_PREFIX_PATH=$PS2SDK/ports")
#CMAKE_OPTIONS=("${CMAKE_OPTIONS[@]}" -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON)

function build {
    START_DIR="${PWD}"
    cd "$1"
    shift
    mkdir -p build
    cd build
    CFLAGS="$CFLAGS" cmake "${CMAKE_OPTIONS[@]}" "$@" "${XTRA_OPTS[@]}" .. || { exit 1; }
    "${MAKECMD}" --quiet -j "$PROC_NR" clean all install || { exit 1; }
    cd "${START_DIR}"
}

## Create a synbolic link for retro-compatibility ps2dev.cmake
ln -sf "$PS2DEV/share/ps2dev.cmake" "$PS2SDK/ps2dev.cmake" || { exit 1; }

##
## Remove build folder
##
rm -rf build
mkdir build
cd build

##
## Clone repos
##
git clone --depth 1 -b v1.2.12 https://github.com/madler/zlib || { exit 1; }
git clone --depth 1 -b v5.4.0 https://github.com/xz-mirror/xz.git || { exit 1; }
git clone --depth 1 -b v1.9.4 https://github.com/lz4/lz4.git || { exit 1; }
git clone --depth 1 -b v1.9.2 https://github.com/nih-at/libzip.git || { exit 1; }
git clone --depth 1 -b v1.6.37 https://github.com/glennrp/libpng || { exit 1; }
git clone --depth 1 -b VER-2-10-4 https://github.com/freetype/freetype || { exit 1; }
git clone --depth 1 -b 0.2.5 https://github.com/yaml/libyaml || { exit 1; }
git clone --depth 1 -b 3.0.3 https://github.com/libjpeg-turbo/libjpeg-turbo || { exit 1; }
git clone --depth 1 -b v1.3.5 https://github.com/xiph/ogg.git || { exit 1; }
git clone --depth 1 -b v1.3.7 https://github.com/xiph/vorbis.git || { exit 1; }
git clone --depth 1 -b v5.7.0-stable https://github.com/wolfSSL/wolfssl.git || { exit 1; }
git clone --depth 1 -b curl-8_7_1 https://github.com/curl/curl.git || { exit 1; }
git clone --depth 1 -b 1.9.5 https://github.com/open-source-parsers/jsoncpp.git || { exit 1; }
# "snprintf" not found in "std" namespace error may occur, so patch that out here.
pushd jsoncpp
sed -i -e 's/std::snprintf/snprintf/' include/json/config.h
popd
git clone --depth 1 -b libxmp-4.6.0 https://github.com/libxmp/libxmp.git || { exit 1; } 
git clone --depth 1 -b v1.4 https://github.com/xiph/opus.git || { exit 1; } 
# We need to clone the whole repo and point to the specific hash for now, 
# till they release a new version with cmake compatibility
git clone https://github.com/xiph/opusfile.git || { exit 1; } 
(cd opusfile && git checkout cf218fb54929a1f54e30e2cb208a22d08b08c889 && cd -) || { exit 1; }
# We need to clone the whole repo and point to the specific hash for now, 
# till they release a new version with cmake compatibility
git clone https://github.com/Konstanty/libmodplug.git || { exit 1; } 
(cd libmodplug && git checkout d1b97ed0020bc620a059d3675d1854b40bd2608d && cd -) || { exit 1; }
# We need to clone the whole repo and point to the specific hash for now, 
# till they release a new version with cmake compatibility
git clone https://github.com/sezero/mikmod.git mikmod-mikmod || { exit 1; } 
(cd mikmod-mikmod && git checkout 096d0711ca3e294564a5c6ec18f5bbc3a2aac016 && cd -) || { exit 1; }
# We need to clone a fork, this is a PR opened for ading cmake support
# https://github.com/xiph/theora/pull/14
git clone --depth 1 -b feature/cmake https://github.com/mcmtroffaes/theora.git || { exit 1; }

# SDL requires to have gsKit
git clone --depth 1 -b v1.3.7 https://github.com/ps2dev/gsKit || { exit 1; } 

# We need to clone the whole repo and point to the specific hash for now,
# till a new version is released after this commit
git clone https://github.com/libsdl-org/SDL.git || { exit 1; }
(cd SDL && git checkout 3b1e0e163ba3933daa9aa19f06a7bb3909e05c8a && cd -) || { exit 1; }
git clone --depth 1 -b release-2.6.3 https://github.com/libsdl-org/SDL_mixer.git || { exit 1; }
git clone --depth 1 -b release-2.6.3 https://github.com/libsdl-org/SDL_image.git || { exit 1; }
git clone --depth 1 -b release-2.20.2 https://github.com/libsdl-org/SDL_ttf.git || { exit 1; }

##
## Build cmake projects
##
PROC_NR=1 build zlib -DUNIX:BOOL=ON # Forcing to compile with -j1 because there is a race condition in zlib
build xz -DTUKLIB_CPUCORES_FOUND=ON -DTUKLIB_PHYSMEM_FOUND=ON -DHAVE_GETOPT_LONG=OFF -DBUILD_TESTING=OFF
build lz4/build/cmake -DLZ4_POSITION_INDEPENDENT_LIB=OFF -DLZ4_BUILD_CLI=OFF -DLZ4_BUILD_LEGACY_LZ4C=OFF
build libzip -DBUILD_TOOLS=OFF -DBUILD_REGRESS=OFF
build libpng -DPNG_SHARED=OFF -DPNG_STATIC=ON
build freetype
build libyaml
build libjpeg-turbo -DENABLE_SHARED=FALSE -DWITH_SIMD=0
build ogg
build vorbis
CFLAGS="-DWOLFSSL_GETRANDOM -DNO_WRITEV" build wolfssl -DWOLFSSL_CRYPT_TESTS=OFF -DWOLFSSL_EXAMPLES=OFF -DWOLFSSL_CURL=ON -DWARNING_C_FLAGS=-w
CFLAGS="-DSIZEOF_LONG=4 -DSIZEOF_LONG_LONG=8 -DNO_WRITEV" build curl -DENABLE_THREADED_RESOLVER=OFF -DCURL_USE_OPENSSL=OFF -DCURL_USE_WOLFSSL=ON -DCURL_DISABLE_SOCKETPAIR=ON -DHAVE_BASENAME=NO -DHAVE_ATOMIC=NO -DENABLE_WEBSOCKETS=ON -DENABLE_IPV6=OFF -DCURL_USE_LIBPSL=OFF -DCURL_USE_LIBSSH2=OFF
build libxmp -DBUILD_SHARED=OFF
build opus
build opusfile -DOP_DISABLE_HTTP=ON -DOP_DISABLE_DOCS=ON -DOP_DISABLE_EXAMPLES=ON
build libmodplug
build mikmod-mikmod/libmikmod -DENABLE_SHARED=0 -DENABLE_DOC=OFF
build jsoncpp -DBUILD_OBJECT_LIBS=OFF -DJSONCPP_WITH_TESTS=OFF -DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF
build theora

# gsKit is mandatory for SDL
build gsKit
build SDL -DCMAKE_POSITION_INDEPENDENT_CODE=OFF -DSDL_TESTS=OFF
build SDL_mixer -DCMAKE_POSITION_INDEPENDENT_CODE=OFF -DSDL2MIXER_DEPS_SHARED=OFF -DSDL2MIXER_MOD_MODPLUG=ON -DSDL2MIXER_MIDI=OFF -DSDL2MIXER_FLAC=OFF -DSDL2MIXER_SAMPLES=OFF
build SDL_image -DCMAKE_POSITION_INDEPENDENT_CODE=OFF
build SDL_ttf -DCMAKE_POSITION_INDEPENDENT_CODE=OFF -DSDL2TTF_SAMPLES=OFF

# Finish
cd ..
