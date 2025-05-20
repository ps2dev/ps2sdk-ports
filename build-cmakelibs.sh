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
# Try to solve windows linking issues
$FETCH 5a82f71ed1dfc0bec044d9702463dbdf84ea3b71 https://github.com/madler/zlib.git &
$FETCH v5.8.1 https://github.com/tukaani-project/xz.git &
$FETCH v1.10.0 https://github.com/lz4/lz4.git &
$FETCH v1.11.3 https://github.com/nih-at/libzip.git &
$FETCH 2.18.0 https://github.com/ImageOptim/libimagequant.git &
$FETCH v1.6.47 https://github.com/pnggroup/libpng.git &
$FETCH VER-2-13-3 https://github.com/freetype/freetype.git &
$FETCH v1.16.0 https://github.com/google/googletest.git &
$FETCH 0.2.5 https://github.com/yaml/libyaml.git &
$FETCH 3.1.0 https://github.com/libjpeg-turbo/libjpeg-turbo.git &
$FETCH v1.3.5 https://github.com/xiph/ogg.git &
$FETCH v1.3.7 https://github.com/xiph/vorbis.git &
$FETCH v5.8.0-stable https://github.com/wolfSSL/wolfssl.git &
$FETCH curl-8_13_0 https://github.com/curl/curl.git &
$FETCH 1.9.6 https://github.com/open-source-parsers/jsoncpp.git &
$FETCH libxmp-4.6.2 https://github.com/libxmp/libxmp.git &
$FETCH v1.5.2 https://github.com/xiph/opus.git &
# We need to clone the whole repo and point to the specific hash for now,
# till they release a new version with cmake compatibility
# we need to clone whole repo because it uses `git describe --tags` for version info
$FETCH cf218fb54929a1f54e30e2cb208a22d08b08c889 https://github.com/xiph/opusfile.git true &
# We need to clone the whole repo and point to the specific hash for now,
# till they release a new version with cmake compatibility
$FETCH d1b97ed0020bc620a059d3675d1854b40bd2608d https://github.com/Konstanty/libmodplug.git &
$FETCH libmikmod-3.3.13 https://github.com/sezero/mikmod.git &
# We need to clone a fork, this is a PR opened for ading cmake support
# https://github.com/xiph/theora/pull/14
$FETCH feature/cmake https://github.com/mcmtroffaes/theora.git &

# gsKit requires libtiff
$FETCH v4.7.0 https://gitlab.com/libtiff/libtiff.git &

# SDL requires to have gsKit
$FETCH v1.4.1 https://github.com/ps2dev/gsKit.git &

# Point to a concrete hash for now, till the SDL team releases a new version
$FETCH d0c2d8bc40a90cc1a763f6cf5397e8c9958a33d8 https://github.com/libsdl-org/SDL.git &
$FETCH release-2.8.1 https://github.com/libsdl-org/SDL_mixer.git &
$FETCH release-2.8.8 https://github.com/libsdl-org/SDL_image.git &
$FETCH release-2.24.0 https://github.com/libsdl-org/SDL_ttf.git &

$FETCH libsmb2-6.2 https://github.com/sahlberg/libsmb2.git &

# We need to clone the whole repo and point to the specific hash for now,
# till a new version is released after this commit
$FETCH 7083138fd401faa391c4f829a86b50fdb9c5c727 https://github.com/lsalzman/enet.git &

# Use wget to download argtable2
wget -c --directory-prefix=build  http://prdownloads.sourceforge.net/argtable/argtable2-13.tar.gz &
$FETCH v3.2.2.f25c624 https://github.com/argtable/argtable3.git &

$FETCH v1.7.3 https://github.com/hyperrealm/libconfig.git &

$FETCH R_2_7_1 https://github.com/libexpat/libexpat.git &

$FETCH v3.7.9 https://github.com/libarchive/libarchive.git &

# wait for fetch jobs to finish
wait

# extract argtable2
tar -xzf build/argtable2-13.tar.gz -C build

# NOTE: jsoncpp
# "snprintf" not found in "std" namespace error may occur, so patch that out here.
pushd build/jsoncpp
sed -i -e 's/std::snprintf/snprintf/' include/json/config.h
popd

# NOTE: xz
# flockfile and funlockfile should be made no-ops
pushd build/xz
sed -i -e 's/defined _WIN32/1/' lib/getopt.c
popd

# NOTE: ARGTABLE2
pushd build/argtable2-13
sed -i -e 's/defined __GNU_LIBRARY__/1/' src/getopt.h
popd

# NOTE: libarchive
pushd build/libarchive
# _timezone used for newlib
sed -i -e 's/defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)/1 || \0/' libarchive/archive_write_set_format_iso9660.c
# 32-bit integer "unsigned long int" prototype not compatible with "unsigned int"
sed -i -e 's/defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L/0 \&\& \0/' libarchive/xxhash.c
popd

###
### Change to the build folder
###
cd build

##
## Build cmake projects
##
build_ee zlib -DUNIX:BOOL=ON -DZLIB_BUILD_EXAMPLES=OFF -DZLIB_BUILD_SHARED=OFF -DINSTALL_PKGCONFIG_DIR="${PS2SDK}/ports/lib/pkgconfig"
build_ee xz -DTUKLIB_CPUCORES_FOUND=ON -DTUKLIB_PHYSMEM_FOUND=ON -DHAVE_GETOPT_LONG=OFF -DBUILD_TESTING=OFF -DXZ_TOOL_XZ=OFF
build_ee lz4/build/cmake -DLZ4_POSITION_INDEPENDENT_LIB=OFF -DLZ4_BUILD_CLI=OFF -DLZ4_BUILD_LEGACY_LZ4C=OFF -DCMAKE_POLICY_VERSION_MINIMUM=3.5
build_ee libzip -DBUILD_TOOLS=OFF -DBUILD_REGRESS=OFF
build_ee libimagequant -DLIB_INSTALL_DIR=lib -DBUILD_WITH_SSE=OFF -DCMAKE_POLICY_VERSION_MINIMUM=3.5
build_ee libpng -DPNG_SHARED=OFF -DPNG_STATIC=ON
build_ee freetype
build_ee googletest -DCMAKE_CXX_FLAGS='-DGTEST_HAS_POSIX_RE=0'
build_ee libyaml -DCMAKE_POLICY_VERSION_MINIMUM=3.5
build_ee libjpeg-turbo -DENABLE_SHARED=FALSE -DWITH_SIMD=0
build_ee ogg -DCMAKE_POLICY_VERSION_MINIMUM=3.5
build_ee vorbis -DCMAKE_POLICY_VERSION_MINIMUM=3.5
CFLAGS="-DWOLFSSL_GETRANDOM -DNO_WRITEV -DXINET_PTON\(...\)=0" build_ee wolfssl -DWOLFSSL_CRYPT_TESTS=OFF -DWOLFSSL_EXAMPLES=OFF -DWOLFSSL_CURL=ON -DWARNING_C_FLAGS=-w
CFLAGS="-DSIZEOF_LONG=4 -DSIZEOF_LONG_LONG=8 -DNO_WRITEV" build_ee curl -DENABLE_THREADED_RESOLVER=OFF -DCURL_USE_OPENSSL=OFF -DCURL_USE_WOLFSSL=ON -DCURL_DISABLE_SOCKETPAIR=ON -DHAVE_BASENAME=NO -DHAVE_ATOMIC=NO -DENABLE_WEBSOCKETS=ON -DENABLE_IPV6=OFF -DCURL_USE_LIBPSL=OFF -DCURL_USE_LIBSSH2=OFF
build_ee libxmp -DBUILD_SHARED=OFF
build_ee opus
build_ee opusfile -DOP_DISABLE_HTTP=ON -DOP_DISABLE_DOCS=ON -DOP_DISABLE_EXAMPLES=ON
build_ee libmodplug -DCMAKE_POLICY_VERSION_MINIMUM=3.5
build_ee mikmod/libmikmod -DENABLE_SHARED=0 -DENABLE_DOC=OFF
build_ee jsoncpp -DBUILD_OBJECT_LIBS=OFF -DJSONCPP_WITH_TESTS=OFF -DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF -DCMAKE_POLICY_VERSION_MINIMUM=3.5
CFLAGS="-Wno-implicit-function-declaration" build_ee theora -DHAVE_STRING_H=ON -DBUILD_EXAMPLES=OFF

# libtiff and libtiff_ps2_addons is mandatory for gsKit
CFLAGS="-Dlfind=bsearch" build_ee libtiff -Dtiff-tools=OFF -Dtiff-tests=OFF -DCMAKE_POLICY_VERSION_MINIMUM=3.5

# gsKit is mandatory for SDL
build_ee gsKit -DCMAKE_POLICY_VERSION_MINIMUM=3.5
build_ee SDL -DCMAKE_POSITION_INDEPENDENT_CODE=OFF -DSDL_TESTS=OFF
build_ee SDL_mixer -DCMAKE_POSITION_INDEPENDENT_CODE=OFF -DSDL2MIXER_DEPS_SHARED=OFF -DSDL2MIXER_MOD_MODPLUG=ON -DSDL2MIXER_OPUS=OFF -DSDL2MIXER_WAVPACK=OFF -DSDL2MIXER_MIDI=OFF -DSDL2MIXER_FLAC=OFF -DSDL2MIXER_SAMPLES=OFF
build_ee SDL_image -DCMAKE_POSITION_INDEPENDENT_CODE=OFF -DSDL2IMAGE_TIF=OFF
build_ee SDL_ttf -DCMAKE_POSITION_INDEPENDENT_CODE=OFF -DSDL2TTF_SAMPLES=OFF

build_ee enet

# Build argtable2
CFLAGS="-Wno-implicit-function-declaration -D__GNU_LIBRARY__" build_ee argtable2-13 -DHAVE_STRINGS_H=ON -DHAVE_STDC_HEADERS=ON -DCMAKE_POLICY_VERSION_MINIMUM=3.5
# Copy manually the argtable2.h header
install -m644 argtable2-13/src/argtable2.h $PS2SDK/ports/include/

build_ee argtable3 -DARGTABLE3_INSTALL_CMAKEDIR="${PS2SDK}/ports/lib/cmake/" -DARGTABLE3_REPLACE_GETOPT=OFF -DARGTABLE3_ENABLE_EXAMPLES=OFF -DARGTABLE3_ENABLE_TESTS=OFF -DCMAKE_POLICY_VERSION_MINIMUM=3.5

build_ee libsmb2 -DCMAKE_POLICY_VERSION_MINIMUM=3.5
build_ee libsmb2 -DPS2RPC=1 -DCMAKE_POLICY_VERSION_MINIMUM=3.5
build_iop libsmb2 -DCMAKE_POLICY_VERSION_MINIMUM=3.5
# Disabling for now, as it has some issues with the IRX build in CPU with several cores
# build_irx libsmb2 -DBUILD_IRX=1 -DCMAKE_POLICY_VERSION_MINIMUM=3.5

CFLAGS="-DHAVE_NEWLOCALE -DHAVE_USELOCALE -DHAVE_FREELOCALE" build_ee libconfig -DBUILD_EXAMPLES=OFF -DBUILD_TESTS=OFF -DCMAKE_POLICY_VERSION_MINIMUM=3.5

CFLAGS="-D_BSD_SOURCE" build_ee libexpat/expat -DEXPAT_BUILD_EXAMPLES=OFF -DEXPAT_BUILD_TESTS=OFF -DEXPAT_SHARED_LIBS=OFF -DEXPAT_BUILD_TOOLS=OFF
build_ee libarchive -DBUILD_SHARED_LIBS=OFF -DENABLE_WERROR=OFF

# Finish
cd ..
