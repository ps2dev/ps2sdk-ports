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

MAKE_OPTIONS=-Wno-dev

function make_ps2 {
    START_DIR="${PWD}"
    DIR="$1"
    shift
    cd "$DIR"
    echo "Building '$DIR' for PS2..."
    CFLAGS="$CFLAGS" "${MAKECMD}" -j "${MAKE_OPTIONS}" "$@" "${XTRA_OPTS}" all install
    cd "${START_DIR}"
}

## Create a symbolic link for retro-compatibility ps2dev.cmake and ps2dev_iop.cmake
(cd "${PS2SDK}" && ln -sf "../share/ps2dev.cmake" "ps2dev.cmake" && cd -)
(cd "${PS2SDK}" && ln -sf "../share/ps2dev_iop.cmake" "ps2dev_iop.cmake" && cd -)

##
## Clone repos
##
$FETCH v1.3.1 https://github.com/madler/zlib &
$FETCH v5.4.4 https://github.com/xz-mirror/xz.git &
$FETCH v1.9.4 https://github.com/lz4/lz4.git &
$FETCH v1.9.2 https://github.com/nih-at/libzip.git &
$FETCH v1.6.44 https://github.com/glennrp/libpng &
$FETCH VER-2-13-3 https://github.com/freetype/freetype &
$FETCH v1.15.2 https://github.com/google/googletest &
$FETCH 0.2.5 https://github.com/yaml/libyaml &
$FETCH 3.0.3 https://github.com/libjpeg-turbo/libjpeg-turbo &
$FETCH v1.3.5 https://github.com/xiph/ogg.git &
$FETCH v1.3.7 https://github.com/xiph/vorbis.git &
$FETCH v5.7.0-stable https://github.com/wolfSSL/wolfssl.git &
$FETCH curl-8_10_1 https://github.com/curl/curl.git &
$FETCH 1.9.6 https://github.com/open-source-parsers/jsoncpp.git &
$FETCH libxmp-4.6.0 https://github.com/libxmp/libxmp.git &
$FETCH v1.5.2 https://github.com/xiph/opus.git &
# We need to clone the whole repo and point to the specific hash for now,
# till they release a new version with cmake compatibility
# we need to clone whole repo because it uses `git describe --tags` for version info
$FETCH 9d718345ce03b2fad5d7d28e0bcd1cc69ab2b166 https://github.com/xiph/opusfile.git true &
# We need to clone the whole repo and point to the specific hash for now,
# till they release a new version with cmake compatibility
$FETCH d1b97ed0020bc620a059d3675d1854b40bd2608d https://github.com/Konstanty/libmodplug.git &
# We need to clone the whole repo and point to the specific hash for now,
# till they release a new version with cmake compatibility
$FETCH 0e5b7443388095d919714cc5d9449d6afccd878e https://github.com/sezero/mikmod.git &
# We need to clone a fork, this is a PR opened for ading cmake support
# https://github.com/xiph/theora/pull/14
$FETCH feature/cmake https://github.com/mcmtroffaes/theora.git &

# gsKit requires libtiff
$FETCH v4.7.0 https://gitlab.com/libtiff/libtiff.git &

# SDL requires to have gsKit
$FETCH v1.3.8 https://github.com/ps2dev/gsKit &

# We need to clone this version 2.30.8 with the hash specified, 
# since the original release doesn´t conatin the patched timer code.
$FETCH 2b2907db18484c4c41a6afa0972accd1c0e84237 https://github.com/libsdl-org/SDL.git &
$FETCH sans_glib https://github.com/DominusExult/fluidsynth-sans-glib.git &
$FETCH release-2.6.3 https://github.com/libsdl-org/SDL_mixer.git &
$FETCH release-2.8.2 https://github.com/libsdl-org/SDL_image.git &
$FETCH release-2.22.0 https://github.com/libsdl-org/SDL_ttf.git &

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

$FETCH R_2_6_3 https://github.com/libexpat/libexpat.git &

$FETCH 0.16.4 https://codeberg.org/tenacityteam/libmad.git &

$FETCH 0.16.3 https://codeberg.org/tenacityteam/libid3tag.git &

# We need to clone the whole repo and point to the specific hash for now,
# till a new version is released after this commit
$FETCH d6f771cb0e2515dea6a84ece6f9078750bbcc938 https://github.com/billagee/aalib-patched.git &
$FETCH 1.6.4 https://github.com/fjtrujy/ps2_drivers.git &
$FETCH master https://github.com/Wolf3s/libtap.git &
$FETCH ee-v5.4.6 https://github.com/ps2dev/lua.git &
$FETCH master https://github.com/ps2dev/ps2stuff.git &
$FETCH master https://github.com/ps2dev/ps2gl.git &
$FETCH v1.0.4 https://github.com/israpps/SIOCookie.git &
$FETCH amusat/modify_genromfs_for_embedded_os https://github.com/andreimusat/genromfs.git

# wait for fetch jobs to finish
wait

# extract argtable2
tar -xzf build/argtable2-13.tar.gz -C build

# NOTE: jsoncpp
# "snprintf" not found in "std" namespace error may occur, so patch that out here.
pushd build/jsoncpp
sed -i -e 's/std::snprintf/snprintf/' include/json/config.h
popd

# NOTE: aalib
# Some standard headers could not be found, so patch that out here.
pushd build/aalib-patched/aalib-1.4rc5
sed -i '1i#include <stdlib.h>'                            \
    src/aa{fire,info,lib,linuxkbd,savefont,test,regist}.c &&
sed -i '1i#include <string.h>'                            \
    src/aa{kbdreg,moureg,test,regist}.c                    
popd

pushd build/genromfs
sed -i -e 's|install: all install-bin install-man|install: all|' Makefile
popd

# NOTE: fluidsynth
# PS2 Port by André "Wolf3s" Guilherme, Ideas and Inspiration by 7dog123.
pushd build/fluidsynth-sans-glib
sed -i -e "s|set(CMAKE_CXX_FLAGS \"\${CMAKE_CXX_FLAGS} -pthread\")|set(CMAKE_CXX_FLAGS \"\${CMAKE_CXX_FLAGS} -lpthread -latomic\")|" CMakeLists.txt &&
sed -i '/if ( NOT BUILD_SHARED_LIBS )/,/endif ( NOT BUILD_SHARED_LIBS )/ s|file ( GLOB EXTRA_STATIC_MODULES .* )||' CMakeLists.txt &&
sed -i '/if(NOT TARGET GLib2::glib-2 OR NOT TARGET GLib2::gthread-2)/,/endif()/d' FluidSynthConfig.cmake.in
popd

# NOTE: libunzip
# Port by KrahJohlito
pushd build/zlib/contrib/minizip
sed -i -e 's|lib_LTLIBRARIES = libminizip.la|lib_LTLIBRARIES = libunzip.la|' Makefile.am
sed -i -e 's|libminizip_la_LDFLAGS = $(AM_LDFLAGS) -version-info 1:0:0 -lz|libunzip_la_LDFLAGS = $(AM_LDFLAGS) -version-info 1:0:0 -lz|' Makefile.am
sed -i -e 's|libminizip_la_SOURCES|libunzip_la_SOURCES|' Makefile.am
sed -i '/    mztools.c/d' Makefile.am
sed -i '/    zip.c/d' Makefile.am
sed -i '/    crypt.h/d' Makefile.am
sed -i '/    mztools.h/d' Makefile.am
sed -i '/    zip.h/d' Makefile.am
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

# libtiff is mandatory for gsKit
CFLAGS="-Dlfind=bsearch" build_ee libtiff -Dtiff-tools=OFF -Dtiff-tests=OFF
# gsKit and ps2_drivers is mandatory for SDL
build_ee gsKit
make_ps2 ps2_drivers
build_ee fluidsynth-sans-glib -Denable=aufile=OFF -Denable-dbus=OFF -Denable-ipv6=OFF -Denable-jack=OFF -Denable-libinstpatch=OFF -Denable-libsndfile=OFF -Denable-midishare=OFF -Denable-network=OFF -Denable-oss=OFF -Denable-oss=OFF -Denable-dsound=OFF -Denable-wasapi=OFF -Denable-waveout=OFF -Denable-winmidi=OFF -Denable-pulseaudio=OFF -Denable-pipewire=OFF -Denable-readline=OFF -Denable-threads=OFF -Denable-openmp=OFF
build_ee SDL -DCMAKE_POSITION_INDEPENDENT_CODE=OFF -DSDL_TESTS=OFF
build_ee SDL_mixer -DCMAKE_POSITION_INDEPENDENT_CODE=OFF -DSDL2MIXER_DEPS_SHARED=OFF -DSDL2MIXER_MIDI_FLUIDSYNTH=ON -DSDL2MIXER_MIDI_TIMIDITY=ON -DSDL2MIXER_MOD_MODPLUG=ON -DSDLMIXER_MIDI_NATIVE=ON -DSDLMIXER_MOD_XMP=ON -DSDL2MIXER_FLAC=OFF -DSDL2MIXER_SAMPLES=OFF
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

build_ee libmad
build_ee libid3tag

##
## Build makefile projects
##
make_ps2 ../aalib
make_ps2 ../romfs
make_ps2 genromfs
install -m755 "genromfs/genromfs" "$PS2SDK/bin"
make_ps2 libtap platform=PS2
make_ps2 lua platform=PS2
make_ps2 ps2stuff
make_ps2 ps2gl
make_ps2 ps2gl/glut
make_ps2 SIOCookie

# Finish
cd ..
