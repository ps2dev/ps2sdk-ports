# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2001-2004, ps2dev - http://www.ps2dev.org
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.
#
# $Id: Makefile.sample 1150 2005-06-12 11:42:08Z pixel $

EE_BIN = framerate.elf
EE_OBJS = TestFramerate.o

EE_INCS = -I../../ -I$(PS2SDK)/ports/include/SDL
EE_LDFLAGS = -L../../lib/ -L$(PS2SDK)/ports/lib -L$(PS2DEV)/gsKit/lib
EE_LIBS = -lSDL_gfx -lsdl -lgskit -ldmakit -lm

all: $(EE_BIN)

clean:
	rm -f *.elf *.o *.a

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal

