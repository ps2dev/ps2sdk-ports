LIBPATH = ../lib
LIB_Z = $(LIBPATH)/libz.a

EE_CFLAGS =
EE_OBJS = adler32.o crc32.o deflate.o infback.o inffast.o inflate.o inftrees.o \
	trees.o zutil.o compress.o uncompr.o gzio.o 

EE_LIB = $(LIB_Z)

EE_INCS  = -I../include -I.

all: $(EE_LIB)

install: all
	mkdir -p $(PS2SDK)/ports/include/zlib
	mkdir -p $(PS2SDK)/ports/lib
	cp -f $(LIB_Z) $(PS2SDK)/ports/lib
	cp -f ../include/*.h $(PS2SDK)/ports/include/zlib

clean:
	rm -f $(EE_LIB) $(EE_OBJS) 

include ../Makefile.pref
include ../Makefile.eeglobal

EE_LIBS += -lmc
