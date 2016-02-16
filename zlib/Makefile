LIB_Z = libz.a

EE_OBJS = adler32.o compress.o crc32.o deflate.o gzclose.o gzlib.o gzread.o \
	gzwrite.o infback.o inffast.o inflate.o inftrees.o trees.o uncompr.o zutil.o
EE_LIB = $(LIB_Z)
EE_INCS  = -I.

ZLIB_TEST_OBJS = test/example.o
ZLIB_TEST_BIN = example.elf
ZLIB_TEST_CFLAGS = -I.
ZLIB_TEST_LIBS = libz.a -lc -lkernel
ZLIB_TEST_LDFLAGS = -L$(PS2SDK)/ee/lib

all: $(EE_LIB) $(ZLIB_TEST_BIN)

$(ZLIB_TEST_BIN) : $(ZLIB_TEST_OBJS) $(PS2SDK)/ee/startup/crt0.o
	$(EE_CC) -mno-crt0 -T$(PS2SDK)/ee/startup/linkfile $(ZLIB_TEST_CFLAGS) \
		-o $(ZLIB_TEST_BIN) $(PS2SDK)/ee/startup/crt0.o $(ZLIB_TEST_OBJS) $(ZLIB_TEST_LDFLAGS) $(ZLIB_TEST_LIBS)
install: all
	mkdir -p $(PS2SDK)/ports/include
	mkdir -p $(PS2SDK)/ports/lib
	cp -f $(LIB_Z) $(PS2SDK)/ports/lib
	cp -f zlib.h $(PS2SDK)/ports/include
	cp -f zconf.h $(PS2SDK)/ports/include

clean:
	rm -f $(EE_LIB) $(EE_OBJS) 

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
