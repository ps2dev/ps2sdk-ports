BINS = \
	testtimer.elf \
	testcdrom.elf \
	testendian.elf \
	testerror.elf \
	testjoystick.elf \
	testver.elf \
	checkkeys.elf

OBJS = \
	testtimer.o \
	testcdrom.o \
	testendian.o \
	testerror.o \
	testjoystick.o \
	testver.o \
	checkkeys.o

EE_INCS = -I../include
EE_LIBS = -L. -lc -L$(GSKIT)/lib -L../lib -lsdl -lcdvd

all: $(BINS)

install:

%.elf : %.o $(PS2SDK)/ee/startup/crt0.o
	$(EE_CC) -nostartfiles -T$(PS2SDK)/ee/startup/linkfile $(EE_LDFLAGS) \
		-o $@ $(PS2SDK)/ee/startup/crt0.o $< $(EE_LIBS)

clean:
	rm -f $(BINS) $(OBJS)

include $(PS2SDK)/Defs.make
include ../../Makefile.eeglobal
