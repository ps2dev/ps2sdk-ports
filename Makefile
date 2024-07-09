LIBS := \
	aalib\
	cmakelibs\
	expat\
	libconfig\
	libconfuse\
	libid3tag\
	libjpeg_ps2_addons\
	libmad\
	libtap\
	libtiff\
	lua\
	madplay\
	ps2stuff\
	ps2gl\
	ps2_drivers\
	romfs\
	sdl\
	sdlgfx\
	sdlimag\
	sdlmixer\
	sdlttf\
	SIOCookie\
	unzip\

LIBS_SAMPLES := \
	aalib\
	lua\

# TODO: Broken samples
# problem seems to be with the C strict mode
# throwing warnings as errors
#	sdl
#	sdlgfx
#	sdlmixer
#	ode
#	romfs

.PHONY: $(LIBS)
all: libraries
libraries: $(LIBS)

$(addprefix clean-, $(LIBS)):
	-$(MAKE) -C $(@:clean-%=%) clean

$(addprefix sample-, $(LIBS_SAMPLES)): $(@:sample-%=%)
	$(MAKE) -C $(@:sample-%=%) sample

clean: $(addprefix clean-, $(LIBS))

samples: $(addprefix sample-, $(LIBS_SAMPLES))

aalib:
	$(MAKE) -C $@
	$(MAKE) -C $@ install

cmakelibs: ps2_drivers libtiff
	./build-cmakelibs.sh

clean-cmakelibs:
	rm -rf ./build

expat:
	$(MAKE) -C $@
	$(MAKE) -C $@ install

libconfig:
	$(MAKE) -C $@
	$(MAKE) -C $@ install

libconfuse:
	./fetch.sh v3.3 https://github.com/libconfuse/libconfuse
	cd $@ && ./autogen.sh
	cd $@ && CFLAGS_FOR_TARGET="-G0 -O2 -gdwarf-2 -gz" ./configure --host=mips64r5900el-ps2-elf --prefix=${PS2SDK}/ports --disable-shared --disable-examples
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install

libid3tag: cmakelibs
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install

libjpeg_ps2_addons: cmakelibs
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install

libmad: cmakelibs
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install

libtap:
	./fetch.sh master https://github.com/ps2dev/libtap
	$(MAKE) -C $@ -f Makefile.PS2 all
	$(MAKE) -C $@ -f Makefile.PS2 install

clean-libtap:
	$(MAKE) -C libtap -f Makefile.PS2 clean

libtiff:
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install

lua:
	./fetch.sh ee-v5.4.6 https://github.com/ps2dev/lua
	$(MAKE) -C $@ all platform=PS2
	$(MAKE) -C $@ install platform=PS2

clean-lua:
	$(MAKE) -C lua clean platform=PS2

sample-lua:
	$(MAKE) -C lua sample platform=PS2

# depends on SjPCM sound library
madplay: cmakelibs libid3tag libmad
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install

# depends on dream2gl and ps2Perf
# Broken
ode:
	$(MAKE) -C $@
	$(MAKE) -C $@ install

ps2_drivers:
	./fetch.sh 1.6.2 https://github.com/fjtrujy/ps2_drivers
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install

ps2stuff:
	./fetch.sh master https://github.com/ps2dev/ps2stuff
	$(MAKE) -C $@ install

ps2gl: ps2stuff
	./fetch.sh master https://github.com/ps2dev/ps2gl
	$(MAKE) -C $@ install
	$(MAKE) -C $@/glut install

clean-ps2gl:
	$(MAKE) -C ps2gl clean
	$(MAKE) -C ps2gl/glut clean

romfs:
	$(MAKE) -C $@
	$(MAKE) -C $@ install

sdl: cmakelibs
	$(MAKE) -C $@
	$(MAKE) -C $@ install

sdlgfx: sdlimage
	$(MAKE) -C $@
	$(MAKE) -C $@ install

sdlimage: cmakelibs libtiff sdl
	$(MAKE) -C $@
	$(MAKE) -C $@ install

sdlmixer: sdl
	$(MAKE) -C $@
	$(MAKE) -C $@ install

sdlttf: sdl cmakelibs
	$(MAKE) -C $@
	$(MAKE) -C $@ install

SIOCookie:
	./fetch.sh v1.0.4 https://github.com/israpps/SIOCookie
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install

unzip: cmakelibs
	$(MAKE) -C $@
	$(MAKE) -C $@ install
