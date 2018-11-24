EE_LIB = libpng.a

EE_LDFLAGS += -L. -L$(PS2SDK)/ports/lib
EE_INCS    += -I./include -I$(PS2SDK)/ports/include
EE_LIBS    += -lz

EE_OBJS = png.o pngset.o pngget.o pngrutil.o pngtrans.o \
          pngwutil.o pngread.o pngrio.o pngwio.o pngwrite.o \
          pngrtran.o pngwtran.o pngmem.o pngerror.o pngpread.o

TEST_OBJS = pngtest.o
TEST_BIN = pngtest.elf
TEST_CFLAGS = -I$(PS2SDK)/ports/include -I./
TEST_LIBS = -lz -lm -lc -lkernel
TEST_LDFLAGS = -L$(PS2SDK)/ee/lib -L$(PS2SDK)/ports/lib libpng.a

all: pnglibconf.h $(EE_LIB) $(TEST_BIN)

pnglibconf.h: pngusr.h
	CPPFLAGS=-DPNG_USER_CONFIG $(MAKE) -f scripts/pnglibconf.mak

$(TEST_BIN) : $(TEST_OBJS) $(PS2SDK)/ee/startup/crt0.o
	$(EE_CC) -mno-crt0 -T$(PS2SDK)/ee/startup/linkfile $(TEST_CFLAGS) \
		-o $(TEST_BIN) $(PS2SDK)/ee/startup/crt0.o $(TEST_OBJS) $(TEST_LDFLAGS) $(TEST_LIBS)

install: all
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/include
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f $(EE_LIB) $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f png.h $(DESTDIR)$(PS2SDK)/ports/include
	cp -f pngconf.h $(DESTDIR)$(PS2SDK)/ports/include
	cp -f pnglibconf.h $(DESTDIR)$(PS2SDK)/ports/include

clean:
	/bin/rm -f pnglibconf.h $(EE_OBJS_LIB) $(EE_OBJS) $(EE_BIN) $(EE_LIB) $(TEST_BIN) $(TEST_OBJS)

include $(PS2SDK)/Defs.make
include ../Makefile.eeglobal
