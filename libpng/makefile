# makefile for libpng.a and ps2 with gcc
# Copyright (C) 1998, 1999, 2002 Greg Roelofs and Glenn Randers-Pehrson
# Copyright (C) 1996, 1997 Andreas Dilger
# For conditions of distribution and use, see copyright notice in png.h

EE_LIB = lib/libpng.a

PNGMAJ = 0
PNGMIN = 1.2.8
PNGVER = $(PNGMAJ).$(PNGMIN)

EE_BIN = pngtest.elf

# Where the zlib library and include files are located.
ZLIBLIB=../zlib/lib
ZLIBINC=../zlib/include


EE_LDFLAGS   += -L. -L$(ZLIBLIB) -L./lib
EE_INCS      += -I./include -I$(ZLIBINC)
EE_LIBS      += -lpng -lz -lc -lm -lmc


# library files
EE_OBJS_LIB = png.o pngset.o pngget.o pngrutil.o pngtrans.o pngwutil.o \
	      pngread.o pngrio.o pngwio.o pngwrite.o pngrtran.o \
	      pngwtran.o pngmem.o pngerror.o pngpread.o

# pngtest files
EE_OBJS = pngtest.o


all: $(EE_LIB) $(EE_BIN)

pngtest: $(EE_BIN)

install: all
	mkdir -p $(PS2SDK)/ports/include
	mkdir -p $(PS2SDK)/ports/lib
	cp -f $(EE_LIB) $(PS2SDK)/ports/lib
	cp -f include/*.h $(PS2SDK)/ports/include

clean:
	/bin/rm -f $(EE_OBJS_LIB) $(EE_OBJS) $(EE_BIN) $(EE_LIB)


include Makefile.pref
include Makefile.eeglobal 
