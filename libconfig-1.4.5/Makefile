##
# Makefile for PS2 port of libconfig
##

EE_INCS += -I./include
EE_CFLAGS +=
EE_LDFLAGS +=
EE_ASFLAGS +=

EE_LIB = lib/libconfig.a
EE_OBJS = src/grammar.o src/libconfig.o src/scanctx.o src/scanner.o src/strbuf.o
EE_LIBS =

all: $(EE_LIB)

install: all
	mkdir -p $(PS2SDK)/ports/include
	mkdir -p $(PS2SDK)/ports/lib
	cp -f $(EE_LIB) $(PS2SDK)/ports/lib
	cp -f include/*.h $(PS2SDK)/ports/include

clean:
	rm -f $(EE_LIB) $(EE_OBJS)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
