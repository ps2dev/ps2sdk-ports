LIBPATH=../../lib
LIB_ICE=$(LIBPATH)/libice.a

EE_CFLAGS = -DDISABLE_TIMERS
EE_CFLAGS += -DNO_SIGNAL_H -DDEBUG_ERROR -DDEBUG_VIDEO -DUSE_MATH_H
EE_CFLAGS += -DENABLE_PS2SDK

ICE_OBJS  = IceAABB.o IceContainer.o IceHPoint.o IceIndexedTriangle.o \
	IceMatrix3x3.o IceMatrix4x4.o IceOBB.o IcePlane.o IcePoint.o \
	IceRandom.o IceRay.o IceRevisitedRadix.o IceSegment.o \
	IceTriangle.o IceUtils.o

EE_INCS  = -I../../include -I../../OPCODE

all: $(LIB_ICE) 

$(LIB_ICE): $(ICE_OBJS)
	$(EE_AR) cru $(LIB_ICE) $(ICE_OBJS)

install: all
	mkdir -p $(PS2SDK)/ports/include/ode
	mkdir -p $(PS2SDK)/ports/lib
	cp -f $(LIB_ICE) $(PS2SDK)/ports/lib

clean:
	rm -f $(LIB_ICE) $(LIB_ICEMAIN) $(ICE_OBJS)

include ../../Makefile.pref
include ../../Makefile.eeglobal

