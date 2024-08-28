LIBS := \
	libconfuse\
	libtimidity\
	external_libs\
	libjpeg_ps2_addons\
	madplay\
	romfs\
	sdl\
	sdlgfx\
	sdlimage\
	sdlmixer\
	sdlttf\

LIBS_SAMPLES := \
	aalib\
	lua\

# TODO: Broken samples
# problem seems to be with the C strict mode
# throwing warnings as errors
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

sample-aalib:
	$(MAKE) -C build/aalib sample

sample-lua:
	$(MAKE) -C build/lua sample platform=PS2

external_libs:
	./build-external-libs.sh

clean-external_libs:
	rm -rf ./build

libconfuse:
	./fetch.sh v3.3 https://github.com/libconfuse/libconfuse
	cd build/$@ && ./autogen.sh
	cd build/$@ && CFLAGS_FOR_TARGET="-G0 -O2 -gdwarf-2 -gz" ./configure --host=mips64r5900el-ps2-elf --prefix=${PS2SDK}/ports --disable-shared --disable-examples
	$(MAKE) -C build/$@ all
	$(MAKE) -C build/$@ install

libtimidity:
	./fetch.sh libtimidity-0.2.7 https://github.com/sezero/libtimidity.git
	cd build/$@ && autoreconf -vfi
	cd build/$@ && CFLAGS_FOR_TARGET="-G0 -O2 -gdwarf-2 -gz" ./configure --host=mips64r5900el-ps2-elf --prefix=${PS2SDK}/ports --disable-shared --enable-static --disable-aotest --disable-ao
	$(MAKE) -C build/$@ all
	$(MAKE) -C build/$@ install

libjpeg_ps2_addons: external_libs
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install

# depends on SjPCM sound library
madplay: external_libs
	$(MAKE) -C $@ all
	$(MAKE) -C $@ install

# depends on dream2gl and ps2Perf
# Broken
ode:
	$(MAKE) -C $@
	$(MAKE) -C $@ install

romfs:
	$(MAKE) -C $@
	$(MAKE) -C $@ install

sdl: external_libs
	$(MAKE) -C $@
	$(MAKE) -C $@ install

sdlgfx: sdlimage
	$(MAKE) -C $@
	$(MAKE) -C $@ install

sdlimage: external_libs sdl
	$(MAKE) -C $@
	$(MAKE) -C $@ install

sdlmixer: sdl
	$(MAKE) -C $@
	$(MAKE) -C $@ install

sdlttf: sdl external_libs
	$(MAKE) -C $@
	$(MAKE) -C $@ install
