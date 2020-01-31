EE_BIN = ucl-test.elf
EE_OBJS = ucl-test.o n2e.o
EE_LDFLAGS += -lucl
EE_INCS   += -I..
EE_LIBS   += -L../lib

all: $(EE_BIN)

clean:
	rm -f $(EE_BIN) $(EE_OBJS)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
