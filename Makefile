.PHONY: aalib expat freetype2 libconfig-1.4.5 libid3tag zlib libjpeg libmad libmikmod libpng libtiff lua madplay ode romfs sdl sdlgfx sdlimage sdlmixer sdlttf stlport ucl

all: expat freetype2 libconfig libid3tag zlib libjpeg libmad libmikmod libpng libtiff lua romfs sdl sdlgfx sdlimage sdlmixer sdlttf stlport ucl
#aalib madplay ode

# Broken
aalib:
	rm -rf $PS2SDK/ports/ && $(MAKE) -C $@
	$(MAKE) -C $@ clean

expat:
	rm -rf $PS2SDK/ports/ && $(MAKE) -C $@ install
	$(MAKE) -C $@ clean

freetype2:
	rm -rf $PS2SDK/ports/ && cd $@; ./SetupPS2.sh

libconfig: libconfig-1.4.5

libconfig-1.4.5:
	rm -rf $PS2SDK/ports/ && $(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

zlib:
	rm -rf $PS2SDK/ports/ && $(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

libid3tag: zlib
	rm -rf $PS2SDK/ports/ && $(MAKE) -C $@ all
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

libjpeg:
	rm -rf $PS2SDK/ports/ && $(MAKE) -C $@ all
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

libmad:
	rm -rf $PS2SDK/ports/ && $(MAKE) -C $@ all
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

libmikmod:
	rm -rf $PS2SDK/ports/ && $(MAKE) -C $@ all
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

libpng: zlib
	rm -rf $PS2SDK/ports/ && $(MAKE) -C $@ all
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

libtiff:
	rm -rf $PS2SDK/ports/ && $(MAKE) -C $@ all
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

lua:
	rm -rf $PS2SDK/ports/ && $(MAKE) -C $@ all
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

# depends on isJPCM
# Broken
madplay: libid3tag libmad
	rm -rf $PS2SDK/ports/ && $(MAKE) -C $@ all
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

# depends on dream2gl and ps2Perf
# Broken
ode:
	rm -rf $PS2SDK/ports/ && $(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

romfs:
	rm -rf $PS2SDK/ports/ && $(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

sdl: libjpeg
	rm -rf $PS2SDK/ports/ && $(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

sdlgfx: sdlimage
	rm -rf $PS2SDK/ports/ && $(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

sdlimage: sdl libpng libtiff
	rm -rf $PS2SDK/ports/ && $(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

sdlmixer: sdl
	rm -rf $PS2SDK/ports/ && $(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

sdlttf: sdl freetype2
	rm -rf $PS2SDK/ports/ && $(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

stlport:
	rm -rf $PS2SDK/ports/ && $(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

ucl:
	rm -rf $PS2SDK/ports/ && $(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean
