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
CONFIGCMD=./configure
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

function make_ee {
    START_DIR="${PWD}"
    DIR="$1"
    shift
    cd "$DIR"
    echo "Building '$DIR' for EE..."
    "${MAKECMD}" -j "$PROC_NR" all install
    cd "${START_DIR}"
}

function make_iop {
    START_DIR="${PWD}"
    DIR="$1"
    shift
    cd "$DIR"
    echo "Building '$DIR' for IOP..."
    "${MAKECMD}" -j "$PROC_NR" "${MAKE_OPTIONS}" "$@" "${XTRA_OPTS}" all install
    cd "${START_DIR}"
}

function make_irx {
    START_DIR="${PWD}"
    DIR="$1"
    shift
    cd "$DIR"
    echo "Building '$DIR' for IRX..."
    "${MAKECMD}" -j "$PROC_NR" "${MAKE_OPTIONS}" "$@" "${XTRA_OPTS}" all install
    cd "${START_DIR}"
}

CONFIGURE_OPTIONS=(--host=mips64r5900el-ps2-elf --prefix=${PS2SDK}/ports --disable-shared --enable-static --disable-examples)
XTRAS_CONF_OPTIONS=""

function configure_ee {
    START_DIR="${PWD}"
    DIR="$1"
    shift
    cd "$DIR"
    echo "Configuring '$DIR' for EE..."
    "${CONFIGCMD}" "${CONFIGURE_OPTIONS[@]}" "$@" "${XTRAS_CONF_OPTIONS[@]}"
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
$FETCH 81abcb7d7a4f48169556f9dc74c71a78ecad0c70 https://github.com/xiph/opusfile.git true &
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

# We need to clone the whole repo and point to the specific hash for now,
# till a new version is released after this commit
$FETCH 1edaad17218d67b567c149badce9ef0fc67f65fa https://github.com/libsdl-org/SDL.git &
# SDL_mixer Requires to have fluidsynth and libtimidity
$FETCH ps2-ee-sans-glib https://github.com/Wolf3s/fluidsynth.git &
$FETCH libtimidity-0.2.7 https://github.com/sezero/libtimidity.git
$FETCH release-2.8.0 https://github.com/libsdl-org/SDL_mixer.git &
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
$FETCH v3.3 https://github.com/libconfuse/libconfuse &
$FETCH 1.6.4 https://github.com/fjtrujy/ps2_drivers &
$FETCH master https://github.com/Wolf3s/libtap.git &
$FETCH ee-v5.4.6 https://github.com/ps2dev/lua &
$FETCH master https://github.com/ps2dev/ps2stuff &
$FETCH master https://github.com/ps2dev/ps2gl &
$FETCH v1.0.4 https://github.com/israpps/SIOCookie &

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
    src/aa{kbdreg,moureg,test,regist}.c                   &&
sed -i '/X11_KBDDRIVER/a#include <X11/Xutil.h>'           \
    src/aaxkbd.c                                          &&
sed -i '/rawmode_init/,/^}/s/return;/return 0;/'          \
    src/aalinuxkbd.c                                      &&
autoconf
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
# ps2_drivers is mandatory aswell for SDL
make_ee ps2_drivers
build_ee SDL -DCMAKE_POSITION_INDEPENDENT_CODE=OFF -DSDL_TESTS=OFF
build_ee fluidsynth -Denable=aufile=OFF -Denable-dbus=OFF -Denable-ladspa=OFF -Denable-ipv6=OFF -Denable-jack=OFF -Denable-libinstpatch=OFF -Denable-libsndfile=OFF -Denable-midishare=OFF -Denable-network=ON -Denable-oss=OFF -Denable-dsound=OFF -Denable-wasapi=OFF -Denable-waveout=OFF -Denable-winmidi=OFF -Denable-sdl2=ON -Denable-pulseaudio=OFF -Denable-pipewire=OFF -Denable-readline=OFF -Denable-threads=ON -Denable-openmp=OFF
autoreconf -vfi ./libtimidity
configure_ee ./libtimidity --disable-aotest --disable-ao
build_ee SDL_mixer -DCMAKE_POSITION_INDEPENDENT_CODE=OFF -DSDL2MIXER_MIDI=ON -DSDL2MIXER_DEPS_SHARED=OFF -DSDL2MIXER_OPUS=ON -DSDL2MIXER_WAVPACK=OFF -DSDL2MIXER_MIDI_TIMIDITY=ON -DSDL2MIXER_MOD_MODPLUG=ON -DSDL2MIXER_FLAC=OFF -DSDL2MIXER_SAMPLES=OFF -DSDL2MIXER_VORBIS=VORBISFILE
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
## Build configure projects
##

autoreconf -vfi ./zlib/contrib/minizip
CFLAGS="-DIOAPI_NO_64 -I${PS2SDK}/ports/include" configure_ee ./zlib/contrib/minizip
#Fix this other day: cd libconfuse && ./autogen.sh . && CFLAGS_FOR_TARGET="-G0 -O2 -gdwarf-2 -gz" configure_ee ./libconfuse

##
## Build makefile projects
##
make_ee ../aalib
make_ee libtap platform=PS2
make_ee lua platform=PS2
make_ee ps2stuff
make_ee ps2gl
make_ee ps2gl/glut
make_ee SIOCookie

# Finish
cd ..
