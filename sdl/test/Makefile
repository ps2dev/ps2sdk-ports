BINS = \
	testendian.elf \
	testerror.elf \
	testjoystick.elf \
	testver.elf

OBJS = 
EE_INCS = -I../include
EE_LIBS = -lc -L$(GSKIT)/lib -L../lib -lsdl -lgskit -ldmakit -lpad -lcdvd

all: $(BINS)

%.elf : %.o $(PS2SDK)/ee/startup/crt0.o
	$(EE_CC) -mno-crt0 -T$(PS2SDK)/ee/startup/linkfile $(EE_LDFLAGS) \
		-o $@ $(PS2SDK)/ee/startup/crt0.o $< $(EE_LIBS)

clean:
	rm -f $(BINS) $(OBJS)

include ../Makefile.pref
include ../Makefile.eeglobal

