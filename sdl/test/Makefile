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
EE_LIBS = -L. -lc -L$(PS2DEV)/gsKit/lib -L../lib -lsdl -lcdvd
TEST_FLAGS = -mno-crt0
# TEST_FLAGS = -nostartfiles

all: $(BINS)

install:

%.elf : %.o $(PS2SDK)/ee/startup/crt0.o
	$(EE_CC) $(TEST_FLAGS) -T$(PS2SDK)/ee/startup/linkfile $(EE_LDFLAGS) \
		-o $@ $(PS2SDK)/ee/startup/crt0.o $< $(EE_LIBS)

clean:
	rm -f $(BINS) $(OBJS)

include $(PS2SDK)/Defs.make
include ../../Makefile.eeglobal
