#!/bin/bash
set -e

## Determine the maximum number of processes that Make can work with.
## Also make preparations for different toolchains
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
FETCH="$SCRIPT_DIR/fetch.sh"
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

EE_CMAKE_OPTIONS=(-Wno-dev "-DCMAKE_TOOLCHAIN_FILE=$PS2DEV/share/ps2dev.cmake" "-DCMAKE_INSTALL_PREFIX=$PS2SDK/ports" -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=RelWithDebInfo "-DCMAKE_PREFIX_PATH=$PS2SDK/ports")
IOP_CMAKE_OPTIONS=("-DCMAKE_TOOLCHAIN_FILE=$PS2DEV/share/ps2dev_iop.cmake" "-DCMAKE_INSTALL_PREFIX=$PS2SDK/ports_iop" -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=RelWithDebInfo "-DCMAKE_PREFIX_PATH=$PS2SDK/ports_iop")
IRX_CMAKE_OPTIONS=("-DCMAKE_TOOLCHAIN_FILE=$PS2DEV/share/ps2dev_iop.cmake" "-DCMAKE_PREFIX_PATH=$PS2SDK/ports_iop/irx" -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=RelWithDebInfo "-DCMAKE_PREFIX_PATH=$PS2SDK/ports_iop/irx")

function build_ee {
    START_DIR="${PWD}"
    DIR="$1"
    shift
    cd "$DIR"
    mkdir -p build_ee
    cd build_ee
    echo "Building '$DIR' for EE..."
    CFLAGS="$CFLAGS" cmake "${EE_CMAKE_OPTIONS[@]}" "$@" "${XTRA_OPTS[@]}" ..
    "${MAKECMD}" -j "$PROC_NR" all install
    cd "${START_DIR}"
}

function build_iop {
    START_DIR="${PWD}"
    DIR="$1"
    shift
    cd "$DIR"
    mkdir -p build_iop
    cd build_iop
    echo "Building '$DIR' for IOP..."
    CFLAGS="$CFLAGS" cmake "${IOP_CMAKE_OPTIONS[@]}" "$@" "${XTRA_OPTS[@]}" ..
    "${MAKECMD}" -j "$PROC_NR" all install
    cd "${START_DIR}"
}

function build_irx {
    START_DIR="${PWD}"
    DIR="$1"
    shift
    cd "$DIR"
    mkdir -p build_irx
    cd build_irx
    echo "Building '$DIR' for IRX..."
    CFLAGS="$CFLAGS" cmake "${IRX_CMAKE_OPTIONS[@]}" "$@" "${XTRA_OPTS[@]}" ..
    "${MAKECMD}" -j "$PROC_NR" all install
    cd "${START_DIR}"
}

## Create a symbolic link for retro-compatibility ps2dev.cmake and ps2dev_iop.cmake
(cd "${PS2SDK}" && ln -sf "../share/ps2dev.cmake" "ps2dev.cmake" && cd -)
(cd "${PS2SDK}" && ln -sf "../share/ps2dev_iop.cmake" "ps2dev_iop.cmake" && cd -)

##
## Clone repos
##
$FETCH v1.3.1 https://github.com/madler/zlib &
$FETCH v5.4.0 https://github.com/xz-mirror/xz.git &
$FETCH v1.9.4 https://github.com/lz4/lz4.git &
$FETCH v1.9.2 https://github.com/nih-at/libzip.git &
$FETCH v1.6.43 https://github.com/glennrp/libpng &
$FETCH VER-2-10-4 https://github.com/freetype/freetype &
$FETCH v1.14.0 https://github.com/google/googletest &
$FETCH 0.2.5 https://github.com/yaml/libyaml &
$FETCH 3.0.3 https://github.com/libjpeg-turbo/libjpeg-turbo &
$FETCH v1.3.5 https://github.com/xiph/ogg.git &
$FETCH v1.3.7 https://github.com/xiph/vorbis.git &
$FETCH v5.7.0-stable https://github.com/wolfSSL/wolfssl.git &
$FETCH curl-8_7_1 https://github.com/curl/curl.git &
$FETCH 1.9.5 https://github.com/open-source-parsers/jsoncpp.git &
$FETCH libxmp-4.6.0 https://github.com/libxmp/libxmp.git &
$FETCH v1.4 https://github.com/xiph/opus.git &
# We need to clone the whole repo and point to the specific hash for now,
# till they release a new version with cmake compatibility
# we need to clone whole repo because it uses `git describe --tags` for version info
$FETCH cf218fb54929a1f54e30e2cb208a22d08b08c889 https://github.com/xiph/opusfile.git true &
# We need to clone the whole repo and point to the specific hash for now,
# till they release a new version with cmake compatibility
$FETCH d1b97ed0020bc620a059d3675d1854b40bd2608d https://github.com/Konstanty/libmodplug.git &
# We need to clone the whole repo and point to the specific hash for now,
# till they release a new version with cmake compatibility
$FETCH 096d0711ca3e294564a5c6ec18f5bbc3a2aac016 https://github.com/sezero/mikmod.git &
# We need to clone a fork, this is a PR opened for ading cmake support
# https://github.com/xiph/theora/pull/14
$FETCH feature/cmake https://github.com/mcmtroffaes/theora.git &

# gsKit requires libtiff
$FETCH v4.6.0 https://gitlab.com/libtiff/libtiff.git &

# SDL requires to have gsKit
$FETCH v1.3.8 https://github.com/ps2dev/gsKit &

# We need to clone the whole repo and point to the specific hash for now,
# till a new version is released after this commit
$FETCH 10c14e78b650e626293aa18155efec54cdee7098 https://github.com/libsdl-org/SDL.git &
$FETCH release-2.6.3 https://github.com/libsdl-org/SDL_mixer.git &
$FETCH release-2.6.3 https://github.com/libsdl-org/SDL_image.git &
$FETCH release-2.20.2 https://github.com/libsdl-org/SDL_ttf.git &

# We need to clone the whole repo and point to the specific hash for now,
# till a new version is released after this commit
$FETCH fb5597bf3852aeb9aef5ca7305e049bfb4c0bb7f https://github.com/sahlberg/libsmb2.git &

# We need to clone the whole repo and point to the specific hash for now,
# till a new version is released after this commit
$FETCH 7083138fd401faa391c4f829a86b50fdb9c5c727 https://github.com/lsalzman/enet.git &

# Use wget to download argtable2
wget -c --directory-prefix=build  http://prdownloads.sourceforge.net/argtable/argtable2-13.tar.gz &
$FETCH v3.2.2.f25c624 https://github.com/argtable/argtable3.git &

$FETCH v1.7.3 https://github.com/hyperrealm/libconfig.git &

$FETCH R_2_6_2 https://github.com/libexpat/libexpat.git &

# wait for fetch jobs to finish
wait

# extract argtable2
tar -xzf build/argtable2-13.tar.gz -C build

# NOTE: jsoncpp
# "snprintf" not found in "std" namespace error may occur, so patch that out here.
pushd build/jsoncpp
sed -i -e 's/std::snprintf/snprintf/' include/json/config.h
popd

###
### Change to the build folder
###
cd build

##
## Build cmake projects
##
PROC_NR=1 build_ee zlib -DUNIX:BOOL=ON -DZLIB_BUILD_EXAMPLES=OFF
build_ee xz -DTUKLIB_CPUCORES_FOUND=ON -DTUKLIB_PHYSMEM_FOUND=ON -DHAVE_GETOPT_LONG=OFF -DBUILD_TESTING=OFF
build_ee lz4/build/cmake -DLZ4_POSITION_INDEPENDENT_LIB=OFF -DLZ4_BUILD_CLI=OFF -DLZ4_BUILD_LEGACY_LZ4C=OFF
build_ee libzip -DBUILD_TOOLS=OFF -DBUILD_REGRESS=OFF
build_ee libpng -DPNG_SHARED=OFF -DPNG_STATIC=ON
build_ee freetype
build_ee googletest -DCMAKE_CXX_FLAGS='-DGTEST_HAS_POSIX_RE=0'
build_ee libyaml
build_ee libjpeg-turbo -DENABLE_SHARED=FALSE -DWITH_SIMD=0
build_ee ogg
build_ee vorbis
CFLAGS="-DWOLFSSL_GETRANDOM -DNO_WRITEV" build_ee wolfssl -DWOLFSSL_CRYPT_TESTS=OFF -DWOLFSSL_EXAMPLES=OFF -DWOLFSSL_CURL=ON -DWARNING_C_FLAGS=-w
CFLAGS="-DSIZEOF_LONG=4 -DSIZEOF_LONG_LONG=8 -DNO_WRITEV" build_ee curl -DENABLE_THREADED_RESOLVER=OFF -DCURL_USE_OPENSSL=OFF -DCURL_USE_WOLFSSL=ON -DCURL_DISABLE_SOCKETPAIR=ON -DHAVE_BASENAME=NO -DHAVE_ATOMIC=NO -DENABLE_WEBSOCKETS=ON -DENABLE_IPV6=OFF -DCURL_USE_LIBPSL=OFF -DCURL_USE_LIBSSH2=OFF
build_ee libxmp -DBUILD_SHARED=OFF
build_ee opus
build_ee opusfile -DOP_DISABLE_HTTP=ON -DOP_DISABLE_DOCS=ON -DOP_DISABLE_EXAMPLES=ON
build_ee libmodplug
build_ee mikmod/libmikmod -DENABLE_SHARED=0 -DENABLE_DOC=OFF
build_ee jsoncpp -DBUILD_OBJECT_LIBS=OFF -DJSONCPP_WITH_TESTS=OFF -DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF
build_ee theora

# libtiff and libtiff_ps2_addons is mandatory for gsKit
CFLAGS="-Dlfind=bsearch" build_ee libtiff -Dtiff-tools=OFF -Dtiff-tests=OFF

# gsKit is mandatory for SDL
build_ee gsKit
build_ee SDL -DCMAKE_POSITION_INDEPENDENT_CODE=OFF -DSDL_TESTS=OFF
build_ee SDL_mixer -DCMAKE_POSITION_INDEPENDENT_CODE=OFF -DSDL2MIXER_DEPS_SHARED=OFF -DSDL2MIXER_MOD_MODPLUG=ON -DSDL2MIXER_MIDI=OFF -DSDL2MIXER_FLAC=OFF -DSDL2MIXER_SAMPLES=OFF
build_ee SDL_image -DCMAKE_POSITION_INDEPENDENT_CODE=OFF
build_ee SDL_ttf -DCMAKE_POSITION_INDEPENDENT_CODE=OFF -DSDL2TTF_SAMPLES=OFF

build_ee enet

# Build argtable2
CFLAGS="-Wno-implicit-function-declaration" build_ee argtable2-13 -DHAVE_STRINGS_H=ON -DHAVE_STDC_HEADERS=ON
# Copy manually the argtable2.h header
install -m644 argtable2-13/src/argtable2.h $PS2SDK/ports/include/

build_ee argtable3 -DARGTABLE3_INSTALL_CMAKEDIR="${PS2SDK}/ports/lib/cmake/" -DARGTABLE3_REPLACE_GETOPT=OFF -DARGTABLE3_ENABLE_EXAMPLES=OFF -DARGTABLE3_ENABLE_TESTS=OFF

build_ee libsmb2
build_ee libsmb2 -DPS2RPC=1
build_iop libsmb2
# Disabling for now, as it has some issues with the IRX build in CPU with several cores
# build_irx libsmb2 -DBUILD_IRX=1

CFLAGS="-DHAVE_NEWLOCALE -DHAVE_USELOCALE -DHAVE_FREELOCALE" build_ee libconfig -DBUILD_EXAMPLES=OFF -DBUILD_TESTS=OFF

CFLAGS="-Darc4random_buf=random -DHAVE_GETRANDOM" build_ee libexpat/expat -DEXPAT_BUILD_EXAMPLES=OFF -DEXPAT_BUILD_TESTS=OFF -DEXPAT_SHARED_LIBS=OFF -DEXPAT_BUILD_TOOLS=OFF

# Finish
cd ..
