# makefile for sdlttf.a 
#------------------------------------------------------------------
# Review README & LICENSE files for further details.
#------------------------------------------------------------------
# GCC 3.2.2 / PS2SDK 1.2 / SDL / FREETYTPE
#------------------------------------------------------------------
# ps2port : brice rouanet <tmator@gmail.com>
# -----------------------------------------------------------------
EE_LIB_DIR = lib/

EE_LIB = $(EE_LIB_DIR)libSDL_net.a

EE_INCS      += -I./include -I$(PS2SDK)/ports/include -I$(PS2SDK)/ports/include/SDL -I$(PS2SDK)/ee/include
EE_LIBS      += -lSDL -lnetman -lps2ip -lcglue

EE_OBJS = SDLnet.o SDLnetselect.o SDLnetTCP.o SDLnetUDP.o

all: $(EE_LIB_DIR) $(EE_LIB)

$(EE_LIB_DIR):
	mkdir -p $(EE_LIB_DIR)

install: all
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/include/SDL
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f $(EE_LIB) $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f SDL_net.h $(DESTDIR)$(PS2SDK)/ports/include/SDL

clean:
	rm -f -r $(EE_OBJS) $(EE_LIB_DIR)

sample:

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
