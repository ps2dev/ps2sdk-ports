# makefile for sdlttf.a 
#------------------------------------------------------------------
# Review README & LICENSE files for further details.
#------------------------------------------------------------------
# GCC 3.2.2 / PS2SDK 1.2 / SDL / FREETYTPE
#------------------------------------------------------------------
# ps2port : brice rouanet <tmator@gmail.com>
# -----------------------------------------------------------------

EE_LIB = lib/libSDL_ttf.a

EE_INCS      += -I./include -I$(PS2SDK)/ports/include -I$(PS2SDK)/ports/include/SDL
EE_LIBS      += -lSDL

EE_OBJS = SDL_ttf.o

all: $(EE_LIB)

install: all
	mkdir -p $(PS2SDK)/ports/include/SDL
	mkdir -p $(PS2SDK)/ports/lib
	cp -f $(EE_LIB) $(PS2SDK)/ports/lib
	cp -f ./include/*.h $(PS2SDK)/ports/include/SDL

clean:
	/bin/rm -f $(EE_OBJS) $(EE_LIB)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal 
