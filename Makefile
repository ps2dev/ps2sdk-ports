.PHONY: submodules aalib expat freetype2 libconfig libid3tag zlib libjpeg libmad libmikmod libpng libtap libtiff lua madplay ode romfs sdl sdlgfx sdlimage sdlmixer sdlttf stlport ucl

ifneq ("$(wildcard $(GSKIT)/include/gsKit.h)","")
all: submodules aalib expat freetype2 libconfig libid3tag zlib libjpeg libmad libmikmod libpng libtiff lua madplay sdl sdlgfx sdlimage sdlmixer sdlttf ucl
# libtap stlport ode
else
all: submodules aalib expat freetype2 libconfig libid3tag zlib libjpeg libmad libmikmod libpng libtiff lua madplay ucl
# libtap stlport ode
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

zlib:
	git submodule update --init zlib
	$(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

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

libpng: zlib
	git submodule update --init libpng	
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

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
#	$(MAKE) -C romfs sample
