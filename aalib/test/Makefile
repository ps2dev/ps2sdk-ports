# ps2aalib
# port basique de la aalib
# tmator@gmail.com
# GPL V2
#

EE_INCS = -I../include -I$(PS2SDK)/ee/include
EE_BIN = aafire.elf
EE_OBJS = aafire.o
EE_LDFLAGS = -L../lib/
EE_LIBS = -lc -lkernel ../lib/libaa.a -ldebug -lm

all: $(EE_BIN)

clean:
	rm -f *.elf *.o *.a

include ../Makefile.pref
include ../Makefile.eeglobal

