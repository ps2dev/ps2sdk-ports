EE_BIN = ../bin/madplay.elf
EE_OBJS = audio.o audio_aiff.o audio_snd.o audio_wave.o audio_raw.o \
	audio_hex.o audio_cdda.o audio_null.o crc.o filter.o getopt.o \
	getopt1.o madplay.o resample.o rgain.o zlib-stub.o player.o \
	strcasecmp.o strncasecmp.o tag.o tagger.o version.o  \
	file.o directory.o bstdfile.o rmalloc.o \
	audio_sjpcm.o

EE_LIBS += -lfileXio -ldebug -lmad -lm -lkernel \
	-lid3tag -lc -lkernel -lmc \
	-lm -lkernel -lgcc -lc -lmc -lkernel -lcdvd -lsjpcm -lz

# excluded: bar.o option.o

EE_CXXFLAGS += -fno-exceptions -fno-rtti

EE_CFLAGS += -O2 -DHAVE_STRNCASECMP -DHAVE_CONFIG_H -DHAVE_UNISTD_H \
	-DHAVE_STRING_H -DHAVE_SYS_TYPES_H

EE_INCS +=  -I../include -Iinclude -I$(LIBMAD)/ee/include \
	-I$(ID3TAG)/ee/include  -I$(ZLIB)/include -I$(ISJPCM)/ee/src/

EE_LDFLAGS += -L$(LIBMAD)/ee/lib -L$(ID3TAG)/ee/lib -L$(ZLIB)/lib \
	-L$(ISJPCM)/ee/lib

all: $(EE_BIN)

clean:
	rm -f *.elf *.o *.a *.s $(EE_BIN)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
