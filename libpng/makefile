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
EE_INCS      += -I$(ZLIBINC)
EE_LIBS      += -lpng -lz -lc -lm -lmc


# library files
EE_OBJS_LIB = png.o pngset.o pngget.o pngrutil.o pngtrans.o pngwutil.o \
	      pngread.o pngrio.o pngwio.o pngwrite.o pngrtran.o \
	      pngwtran.o pngmem.o pngerror.o pngpread.o

# pngtest files
EE_OBJS = pngtest.o


all: $(EE_LIB) $(EE_BIN)

pngtest: $(EE_BIN)

clean:
	/bin/rm -f $(EE_OBJS_LIB) $(EE_OBJS) $(EE_BIN) $(EE_LIB)

# DO NOT DELETE THIS LINE -- make depend depends on it.

png.o png.pic.o: png.h pngconf.h
pngerror.o pngerror.pic.o: png.h pngconf.h
pngrio.o pngrio.pic.o: png.h pngconf.h
pngwio.o pngwio.pic.o: png.h pngconf.h
pngmem.o pngmem.pic.o: png.h pngconf.h
pngset.o pngset.pic.o: png.h pngconf.h
pngget.o pngget.pic.o: png.h pngconf.h
pngread.o pngread.pic.o: png.h pngconf.h
pngrtran.o pngrtran.pic.o: png.h pngconf.h
pngrutil.o pngrutil.pic.o: png.h pngconf.h
pngtrans.o pngtrans.pic.o: png.h pngconf.h
pngwrite.o pngwrite.pic.o: png.h pngconf.h
pngwtran.o pngwtran.pic.o: png.h pngconf.h
pngwutil.o pngwutil.pic.o: png.h pngconf.h
pngpread.o pngpread.pic.o: png.h pngconf.h

pngtest.o: png.h pngconf.h

include Makefile.pref
include Makefile.eeglobal 
