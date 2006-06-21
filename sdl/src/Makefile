LIBPATH=../lib
LIB_SDL=$(LIBPATH)/libsdl.a
LIB_SDLMAIN=$(LIBPATH)/libsdlmain.a


# Simulate haRdware Surface
SDL_USE_HW_SURFACE=1

# Enable USB keyboard and mouse (100 KB)
HAVE_INPUT_DEVICES=0

# Enable Multitap Support (Uses X* IOP modules)
ENABLE_MTAP=0

# Enable FREESD instead of LIBSD
USE_FREESD=1


EE_CFLAGS  = -DNO_SIGNAL_H -DDEBUG_ERROR -DUSE_MATH_H
EE_CFLAGS += -DENABLE_PS2SDK

ifeq ($(ENABLE_MTAP), 1)
EE_CFLAGS += -DPS2SDL_ENABLE_MTAP
endif

ifeq ($(HAVE_INPUT_DEVICES), 1)
EE_CFLAGS += -DPS2SDL_USE_INPUT_DEVICES
endif

ifeq ($(USE_FREESD), 1)
EE_CFLAGS += -DUSE_FREESD
endif

ifeq ($(ENABLE_VSYNC_CB), 1)
EE_CFLAGS += -DENABLE_VSYNC_CB
endif

ifeq ($(SDL_USE_HW_SURFACE), 1)
EE_CFLAGS += -DSDL_USE_HW_SURFACE
endif



EE_INCS  = -I../include
EE_INCS += -Ivideo -Ithread -Ievents -Itimer -Iaudio -Ijoystick -Icdrom
EE_INCS += -I$(PS2SDK)/ports/include -I$(PS2DEV)/gsKit/include
EE_LIBS  = 

all: $(LIB_SDL) $(LIB_SDLMAIN)

# generic SDL objects
SDL_OBJS  = SDL.o SDL_error.o SDL_fatal.o SDL_getenv.o SDL_loadso.o
SDL_OBJS += video/SDL_video.o video/SDL_surface.o video/SDL_pixels.o
SDL_OBJS += video/SDL_blit.o video/SDL_blit_0.o video/SDL_blit_1.o
SDL_OBJS += video/SDL_blit_N.o video/SDL_blit_A.o 
SDL_OBJS += video/SDL_RLEaccel.o video/SDL_cursor.o video/SDL_bmp.o
SDL_OBJS += video/SDL_yuv.o video/SDL_yuv_sw.o video/SDL_stretch.o
SDL_OBJS += thread/SDL_thread.o 
SDL_OBJS += events/SDL_events.o events/SDL_quit.o events/SDL_active.o
SDL_OBJS += events/SDL_keyboard.o events/SDL_mouse.o events/SDL_resize.o
SDL_OBJS += thread/generic/SDL_syscond.o
SDL_OBJS += thread/generic/SDL_sysmutex.o
SDL_OBJS += audio/SDL_wave.o audio/SDL_audio.o audio/SDL_audiomem.o
SDL_OBJS += audio/SDL_audiocvt.o audio/SDL_mixer.o
SDL_OBJS += file/SDL_rwops.o
SDL_OBJS += cdrom/SDL_cdrom.o 
SDL_OBJS += timer/SDL_timer.o 
SDL_OBJS += joystick/SDL_joystick.o 
SDL_OBJS += endian/SDL_endian.o

# PS2SDK specific
SDL_OBJS += main/ps2sdk/sdl_main.o
SDL_OBJS += thread/ps2sdk/SDL_syssem.o
SDL_OBJS += thread/ps2sdk/SDL_systhread.o
SDL_OBJS += timer/ps2sdk/SDL_systimer.o
SDL_OBJS += video/ps2sdk/SDL_ps2video.o
SDL_OBJS += audio/ps2sdk/SDL_ps2audio.o 
SDL_OBJS += cdrom/ps2sdk/SDL_syscdrom.o
SDL_OBJS += joystick/ps2sdk/ps2joystick.o

# embedded objects
SDL_OBJS += audio/ps2sdk/audsrv_irx.o 
ifeq ($(USE_FREESD), 1)
SDL_OBJS += audio/ps2sdk/freesd_irx.o
endif

ifeq ($(HAVE_INPUT_DEVICES), 1)
SDL_OBJS += video/ps2sdk/SDL_ps2kbdevents.o
SDL_OBJS += video/ps2sdk/SDL_ps2mouseevents.o
SDL_OBJS += video/ps2sdk/SDL_ps2USBevents.o

SDL_OBJS += video/ps2sdk/ps2kbd_irx.o 
SDL_OBJS += video/ps2sdk/ps2usbd_irx.o 
SDL_OBJS += video/ps2sdk/ps2mouse_irx.o 
endif

SDLMAIN_OBJS = main/arch.o main/ps2sdk/sdl_main.o

# inline

ifeq ($(USE_FREESD), 1)
audio/ps2sdk/freesd_irx.o: $(PS2SDK)/iop/irx/freesd.irx
	$(PS2SDK)/bin/bin2o $< $@ freesd_irx
endif

audio/ps2sdk/audsrv_irx.o: $(PS2SDK)/iop/irx/audsrv.irx
	$(PS2SDK)/bin/bin2o $< $@ audsrv_irx

video/ps2sdk/ps2kbd_irx.o: $(PS2SDK)/iop/irx/ps2kbd.irx
	$(PS2SDK)/bin/bin2o $< $@ ps2kbd_irx

video/ps2sdk/ps2mouse_irx.o: $(PS2SDK)/iop/irx/ps2mouse.irx
	$(PS2SDK)/bin/bin2o $< $@ ps2mouse_irx

video/ps2sdk/ps2usbd_irx.o: $(PS2SDK)/iop/irx/usbd.irx
	$(PS2SDK)/bin/bin2o $< $@ ps2usbd_irx


ifeq ($(ENABLE_MTAP), 1)
EMBEDDED = \
	$(PS2SDK)/ee/lib/libpadx.a           \
	$(PS2SDK)/ee/lib/libmtap.a
else
EMBEDDED = \
	$(PS2SDK)/ee/lib/libpad.a
endif

EMBEDDED += \
	$(PS2SDK)/ee/lib/libcdvd.a	\
	$(PS2SDK)/ee/lib/libpatches.a       \
	$(PS2SDK)/ee/lib/libkbd.a       \
	$(PS2SDK)/ee/lib/libmouse.a     \
	$(PS2SDK)/ee/lib/libaudsrv.a    \
	$(PS2SDK)/ports/lib/libjpg.a    \
	$(PS2DEV)/gsKit/lib/libdmakit.a	\
	$(PS2DEV)/gsKit/lib/libgskit.a

$(LIB_SDL): $(SDL_OBJS)
	# packing with all embedded libraries
	rm -rf tmp
	mkdir tmp
	$(foreach f, $(EMBEDDED), (cd tmp; $(AR) x $f);)
	cp $(SDL_OBJS) tmp
	$(EE_AR) cru $(LIB_SDL) tmp/*
	rm -rf tmp

$(LIB_SDLMAIN): $(SDLMAIN_OBJS)
	$(EE_AR) cru $(LIB_SDLMAIN) $(SDLMAIN_OBJS)

install: all
	mkdir -p $(PS2SDK)/ports/include/SDL
	mkdir -p $(PS2SDK)/ports/lib
	cp -f $(LIB_SDL) $(LIB_SDLMAIN) $(PS2SDK)/ports/lib
	cp -f ../include/*.h $(PS2SDK)/ports/include/SDL

clean:
	rm -f $(LIB_SDL) $(LIB_SDLMAIN) $(SDL_OBJS)

include ../Makefile.pref
include ../Makefile.eeglobal

