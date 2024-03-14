# makefile for libSDL_gfx.a 
#------------------------------------------------------------------
# Review README & LICENSE files for further details.
#------------------------------------------------------------------
# GCC 3.2.2 / PS2SDK 1.2 / SDL / SDL_IMAGE
#------------------------------------------------------------------
# ps2port : brice rouanet <tmator@gmail.com>
# http://www.psxdev.org => froggies team
# -----------------------------------------------------------------
EE_OBJS_DIR = obj/
EE_LIB_DIR = lib/

EE_LIB = $(EE_LIB_DIR)libSDL_gfx.a

EE_INCS      += -I./include -I$(PS2SDK)/ports/include -I$(PS2SDK)/ports/include/SDL
EE_LIBS      += -lSDL

EE_OBJS =	SDL_framerate.o	\
        SDL_gfxBlitFunc.o \
		SDL_gfxPrimitives.o	\
		SDL_imageFilter.o	\
		SDL_rotozoom.o
EE_OBJS := $(EE_OBJS:%=$(EE_OBJS_DIR)%)

all: $(EE_OBJS_DIR) $(EE_LIB_DIR) $(EE_LIB)

$(EE_OBJS_DIR):
	mkdir -p $(EE_OBJS_DIR)

$(EE_LIB_DIR):
	mkdir -p $(EE_LIB_DIR)

$(EE_OBJS_DIR)%.o : %.c
	$(EE_C_COMPILE) -c $< -o $@

sample: all
	$(MAKE) -C Ps2-Test all

install: all
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/include/SDL
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f $(EE_LIB) $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f *.h $(DESTDIR)$(PS2SDK)/ports/include/SDL

clean:
	rm -f -r $(EE_OBJS_DIR) $(EE_LIB_DIR)
	$(MAKE) -C Ps2-Test clean

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
