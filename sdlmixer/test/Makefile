BINS = sample.elf

EE_CFLAGS = -DUSE_RWOPS
EE_INCS = -I../include -I$(PS2SDK)/ports/include
EE_LIBS = -L../lib -L$(PS2SDK)/ports/lib -lsdlmixer -lsdl

all: $(BINS)

install:

%.elf : %.o $(PS2SDK)/ee/startup/crt0.o
	$(EE_CC) -mno-crt0 -T$(PS2SDK)/ee/startup/linkfile $(EE_LDFLAGS) \
		-o $@ $(PS2SDK)/ee/startup/crt0.o $< $(EE_LIBS)

clean:
	rm -f $(BINS) $(OBJS)

include ../Makefile.pref
include ../Makefile.eeglobal

