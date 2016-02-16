EE_BIN = ucl-test.elf
EE_OBJS = ucl-test.o n2e.o
EE_LDFLAGS += -lucl

all: $(EE_BIN)

clean:
	rm -f $(EE_BIN) $(EE_OBJS)

include $(PS2SDK)/Defs.make
include ../../Makefile.eeglobal
