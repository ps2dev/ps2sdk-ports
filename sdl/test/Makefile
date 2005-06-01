BINS = \
	testtimer.elf \
	testcdrom.elf \
	testendian.elf \
	testerror.elf \
	testjoystick.elf \
	testver.elf \
	checkkeys.elf

OBJS = 
EE_INCS = -I../include
EE_LIBS = -L. -lc -L$(PS2DEV)/gsKit/lib -L../lib -lsdl -lcdvd

all: $(BINS)

install:

%.elf : %.o $(PS2SDK)/ee/startup/crt0.o
	$(EE_CC) -mno-crt0 -T$(PS2SDK)/ee/startup/linkfile $(EE_LDFLAGS) \
		-o $@ $(PS2SDK)/ee/startup/crt0.o $< $(EE_LIBS)

clean:
	rm -f $(BINS) $(OBJS)

include ../Makefile.pref
include ../Makefile.eeglobal

