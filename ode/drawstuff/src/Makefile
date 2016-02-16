LIBPATH=../../lib
LIB_DRAWSTUFF=$(LIBPATH)/libdrawstuff.a

EE_CFLAGS = -DDISABLE_TIMERS
EE_CFLAGS += -DNO_SIGNAL_H -DDEBUG_ERROR -DDEBUG_VIDEO -DUSE_MATH_H
EE_CFLAGS += -DENABLE_PS2SDK

DRAWSTUFF_OBJS  = drawstuff.o dreamgl.o pad.o

EE_INCS  = -I../../include -I../../OPCDRAWSTUFF -I$(PS2DEV)/dreamgl/include 

all: $(LIB_DRAWSTUFF) 

$(LIB_DRAWSTUFF): $(DRAWSTUFF_OBJS)
	$(EE_AR) cru $(LIB_DRAWSTUFF) $(DRAWSTUFF_OBJS)

install: all
	mkdir -p $(PS2SDK)/ports/include/drawstuff
	mkdir -p $(PS2SDK)/ports/lib
	cp -f $(LIB_DRAWSTUFF) $(PS2SDK)/ports/lib
	cp -f ../../include/drawstuff/*.h $(PS2SDK)/ports/include/drawstuff

clean:
	rm -f $(LIB_DRAWSTUFF) $(LIB_DRAWSTUFFMAIN) $(DRAWSTUFF_OBJS)

include ../../Makefile.pref
include ../../Makefile.eeglobal

