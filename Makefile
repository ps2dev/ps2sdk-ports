.PHONY: submodules aalib expat freetype2 libconfig libid3tag zlib libjpeg libmad libmikmod libpng libtap libtiff lua madplay ode romfs sdl sdlgfx sdlimage sdlmixer sdlttf stlport ucl

ifneq ("$(wildcard $(GSKIT)/include/gsKit.h)","")
all: submodules aalib expat freetype2 libconfig libid3tag zlib libjpeg libmad libmikmod libpng libtap libtiff lua madplay romfs sdl sdlgfx sdlimage sdlmixer sdlttf stlport ucl
# ode
else
all: submodules aalib expat freetype2 libconfig libid3tag zlib libjpeg libmad libmikmod libpng libtap libtiff lua madplay romfs stlport ucl
# ode
	@echo "GSKIT not set and gsKit not installed.\nSDL libraries were not built."
endif

submodules:
	git submodule init && git submodule update

aalib:
	$(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

expat:
	$(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

freetype2:
	git submodule update --init freetype2	
	cd $@; ./SetupPS2.sh

libconfig:
	$(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

ZLIB_FLAGS = --static --includedir=$(PS2SDK)/ee/include --libdir=$(PS2SDK)/ee/lib --prefix=$(PS2SDK)/ports
zlib:
	git submodule update --init zlib
	cd $@/src && CHOST=ee CFLAGS="-O2 -G0" ./configure $(ZLIB_FLAGS)
	$(MAKE) -C $@/src clean
	$(MAKE) -C $@/src all
	$(MAKE) -C $@/src install

libid3tag: zlib
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

libjpeg:
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

libmad:
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

libmikmod:
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

LIBPNG_FLAGS = --host=mips64el --enable-static=true --enable-shared=false CC=ee-gcc AR=ee-ar STRIP=ee-strip RANLIB=ee-ranlib 
LIBPNG_FLAGS += CFLAGS="-O2 -G0" CPPFLAGS="-I$(PS2SDK)/ee/include -I$(PS2SDK)/common/include -I$(PS2SDK)/ports/include" 
LIBPNG_FLAGS += LDFLAGS="-L$(PS2SDK)/ee/lib -L$(PS2SDK)/ports/lib" --prefix=$(PS2SDK)/ports
libpng: zlib
	git submodule update --init libpng
	cd $@/src && ./configure $(LIBPNG_FLAGS)
	$(MAKE) -C $@/src clean
	$(MAKE) -C $@/src all
	$(MAKE) -C $@/src install

libtap:
	git submodule update --init libtap
	$(MAKE) -C $@ -f Makefile.PS2 all
	$(MAKE) -C $@ -f Makefile.PS2 install
	$(MAKE) -C $@ -f Makefile.PS2 clean

libtiff:
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

lua:
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

# depends on SjPCM sound library
madplay: libid3tag libmad
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

# depends on dream2gl and ps2Perf
# Broken
ode:
	$(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

romfs:
	$(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

sdl: libjpeg
	$(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

sdlgfx: sdlimage
	$(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

sdlimage: sdl libpng libtiff
	$(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

sdlmixer: sdl
	$(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

sdlttf: sdl freetype2
	$(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

# Broken
stlport:
	$(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

ucl:
	$(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

sample:
	$(MAKE) -C aalib sample
	$(MAKE) -C libmikmod sample
	$(MAKE) -C libpng sample
	$(MAKE) -C sdl sample
	$(MAKE) -C sdlgfx sample
	$(MAKE) -C sdlmixer sample
	$(MAKE) -C zlib sample
	$(MAKE) -C ucl sample
# Broken samples
#	$(MAKE) -C lua sample
#	$(MAKE) -C ode sample
#	$(MAKE) -C romfs sample
