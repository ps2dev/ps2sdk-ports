EE_LIB = ../lib/libmad.a
EE_OBJS = bit.o decoder.o fixed.o frame.o huffman.o layer12.o layer3.o \
	stream.o synth.o timer.o version.o 

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal

EE_CFLAGS += -DHAVE_CONFIG_H -DHAVE_SYS_TYPES_H -DHAVE_ERRNO_H -DHAVE_UNISTD_H \
	-DHAVE_FCNTL_H -I$(PS2SDK)/common/include/sys/  -I../include

all: $(EE_BIN)

clean:
	rm -f *.elf *.o $(EE_LIB)

