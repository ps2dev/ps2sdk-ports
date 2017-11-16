MAKE = make

.PHONY: aalib expat freetype-2.4.12 libconfig-1.4.5 libid3tag zlib libjpeg libmad libmikmod libpng libtiff lua madplay ode romfs sdl sdlgfx sdlimage sdlmixer ucl

all: expat freetype-2.4.12 libconfig-1.4.5 zlib libid3tag libjpeg libmad libmikmod libpng libtiff lua romfs sdlgfx sdlttf stlport ucl

# Broken
aalib:
	$(MAKE) -C $@

expat:
	$(MAKE) -C $@ install

freetype-2.4.12:
	cd $@; ./SetupPS2.sh

libconfig-1.4.5:
	$(MAKE) -C $@
	$(MAKE) -C $@ install

zlib:
	$(MAKE) -C $@
	$(MAKE) -C $@ install

libid3tag: zlib
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install

libjpeg:
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install

libmad:
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install

libmikmod:
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install

libpng:
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install

libtiff:
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install

lua:
	$(MAKE) -C $@ -f Makefile.ps2 all
	$(MAKE) -C $@ -f Makefile.ps2 install

# Broken
madplay: libid3tag libmad
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install

# Broken
ode:
	$(MAKE) -C $@
	$(MAKE) -C $@ install

romfs:
	$(MAKE) -C $@
	$(MAKE) -C $@ install

sdl: libjpeg
	$(MAKE) -C $@
	$(MAKE) -C $@ install

sdlgfx: sdl sdlimage
	$(MAKE) -C $@
	$(MAKE) -C $@ install

sdlimage: sdl
	$(MAKE) -C $@
	$(MAKE) -C $@ install

sdlmixer: sdl
	$(MAKE) -C $@
	$(MAKE) -C $@ install

sdlttf:
	$(MAKE) -C $@
	$(MAKE) -C $@ install

stlport:
	$(MAKE) -C $@
	$(MAKE) -C $@ install

ucl:
	$(MAKE) -C $@
	$(MAKE) -C $@ install
