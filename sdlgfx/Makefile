# makefile for libSDL_gfx.a 
#------------------------------------------------------------------
# Review README & LICENSE files for further details.
#------------------------------------------------------------------
# GCC 3.2.2 / PS2SDK 1.2 / SDL / SDL_IMAGE
#------------------------------------------------------------------
# ps2port : brice rouanet <tmator@gmail.com>
# http://www.psxdev.org => froggies team
# -----------------------------------------------------------------

EE_LIB = lib/libSDL_gfx.a

EE_INCS      += -I./include -I$(PS2SDK)/ports/include -I$(PS2SDK)/ports/include/SDL
EE_LIBS      += -lSDL

EE_OBJS = 	SDL_framerate.o 	\
		SDL_gfxPrimitives.o	\
		SDL_imageFilter.o	\
		SDL_rotozoom.o

all: $(EE_LIB)
	$(MAKE) -C  Test all

install: all
	mkdir -p $(PS2SDK)/ports/include/SDL
	mkdir -p $(PS2SDK)/ports/lib
	cp -f $(EE_LIB) $(PS2SDK)/ports/lib
	cp -f *.h $(PS2SDK)/ports/include/SDL

clean:
	/bin/rm -f $(EE_OBJS) $(EE_LIB)
	$(MAKE) -C  Test clean

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal 
