EE_CFLAGS =
EE_LIBS = -L../lib -lz 
EE_OBJS = example.o 
EE_BIN = example.elf

EE_INCS  = -I../include -I.

all: $(EE_BIN)

install: 

clean:
	rm -f $(EE_BIN) $(EE_OBJS) 

include ../Makefile.pref
include ../Makefile.eeglobal

