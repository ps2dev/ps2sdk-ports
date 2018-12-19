.PHONY: aalib expat freetype2 libconfig libid3tag zlib libjpeg libmad libmikmod libpng libtap libtiff lua madplay ode romfs sdl sdlgfx sdlimage sdlmixer sdlttf stlport ucl

all: aalib expat freetype2 libconfig libid3tag zlib libjpeg libmad libmikmod libpng libtiff lua romfs sdl sdlgfx sdlimage sdlmixer sdlttf ucl
# libtap stlport madplay ode

aalib:
	$(MAKE) -C $@
	$(MAKE) -C $@ clean

expat:
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

freetype2:
	cd $@; ./SetupPS2.sh

libconfig:
	$(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

zlib:
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
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

libtap:
	git submodule init && git submodule update
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

libtiff:
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

lua:
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

# depends on isJPCM
# Broken
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
