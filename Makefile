.PHONY: aalib cmakelibs expat libconfig libconfuse libid3tag libjpeg_ps2_addons libmad libtap libtiff lua madplay ode ps2stuff ps2gl ps2_drivers romfs sdl sdlgfx sdlimage sdlmixer sdlttf SIOCookie unzip

all: libraries
libraries: aalib cmakelibs expat libconfig libconfuse libid3tag libjpeg_ps2_addons libmad libtap libtiff lua madplay ps2stuff ps2gl ps2_drivers romfs sdl sdlgfx sdlimage sdlmixer sdlttf SIOCookie unzip

aalib:
	$(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

cmakelibs: ps2_drivers libtiff
	./build-cmakelibs.sh

expat:
	$(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

libconfig:
	$(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

libconfuse:
	rm -rf $@
	git clone --depth 1 -b v3.3 https://github.com/libconfuse/libconfuse
	cd $@ && ./autogen.sh
	cd $@ && CFLAGS_FOR_TARGET="-G0 -O2 -gdwarf-2 -gz" ./configure --host=mips64r5900el-ps2-elf --prefix=$(PS2DEV)/portlibs/ps2 --disable-shared --disable-examples
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

libid3tag: cmakelibs
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

libjpeg_ps2_addons: cmakelibs
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

libmad: cmakelibs
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

libtap:
	rm -rf $@
	git clone --depth 1 https://github.com/ps2dev/libtap
	$(MAKE) -C $@ -f Makefile.PS2 all
	$(MAKE) -C $@ -f Makefile.PS2 install
	$(MAKE) -C $@ -f Makefile.PS2 clean

libtiff:
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

lua:
	rm -rf $@
	git clone --depth 1 -b ee-v5.4.6 https://github.com/ps2dev/lua
	$(MAKE) -C $@ all platform=PS2
	$(MAKE) -C $@ install platform=PS2
	$(MAKE) -C $@ clean platform=PS2

# depends on SjPCM sound library
madplay: cmakelibs libid3tag libmad
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

# depends on dream2gl and ps2Perf
# Broken
ode:
	$(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

ps2_drivers:
	rm -rf $@
	git clone --depth 1 -b 1.6.1 https://github.com/fjtrujy/ps2_drivers
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

ps2stuff:
	rm -rf $@
	git clone --depth 1 https://github.com/ps2dev/ps2stuff
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

ps2gl: ps2stuff
	rm -rf $@
	git clone --depth 1 https://github.com/ps2dev/ps2gl
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean
	$(MAKE) -C $@/glut install 
	$(MAKE) -C $@/glut clean
 

romfs:
	$(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

sdl: cmakelibs
	$(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

sdlgfx: sdlimage
	$(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

sdlimage: cmakelibs libtiff sdl
	$(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

sdlmixer: sdl
	$(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

sdlttf: sdl cmakelibs
	$(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

SIOCookie:
	rm -rf $@
	git clone --depth 1 -b v1.0.4 https://github.com/israpps/SIOCookie
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

unzip: cmakelibs
	$(MAKE) -C $@
	$(MAKE) -C $@ install
	$(MAKE) -C $@ clean

sample: aalib sdl sdlgfx sdlmixer lua ode romfs
	$(MAKE) -C aalib sample
	$(MAKE) -C sdl sample
	$(MAKE) -C sdlgfx sample
	$(MAKE) -C sdlmixer sample
	$(MAKE) -C lua sample platform=PS2
# Broken samples
#	$(MAKE) -C ode sample
#	$(MAKE) -C romfs sample
