BINS = sample.elf

EE_CFLAGS = -DUSE_RWOPS
EE_INCS = -I../include -I$(PS2SDK)/ports/include -I$(PS2SDK)/ports/include/SDL
EE_LIBS = -L../lib -L$(PS2SDK)/ports/lib -L$(PS2DEV)/gsKit/lib -lsdl -lsdlmixer -lsdl -lgskit -ldmakit -ldma 

all: $(BINS)

install:

%.elf : %.o
	$(EE_CXX) $(EE_LDFLAGS) -o $@ $< $(EE_LIBS)

clean:
	rm -f $(BINS) $(OBJS)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
