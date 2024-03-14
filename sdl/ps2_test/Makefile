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
EE_LIBS = -L. -L$(GSKIT)/lib -L../lib -lsdl -lcdvd

ifneq ("$(wildcard $(GSKIT)/include/gsKit.h)","")
all: $(BINS)
else
all:
	@echo "GSKIT not set and gsKit not installed"
endif

install:

%.elf : %.o
	$(EE_CC) $(EE_LDFLAGS) -o $@ $< $(EE_LIBS)

clean:
	rm -f $(BINS) $(OBJS)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
