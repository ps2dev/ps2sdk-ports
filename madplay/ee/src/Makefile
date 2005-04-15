EE_BIN = ../bin/madplay.elf
EE_OBJS = audio.o audio_aiff.o audio_snd.o audio_wave.o audio_raw.o \
	audio_hex.o audio_cdda.o audio_null.o crc.o filter.o getopt.o \
	getopt1.o madplay.o resample.o rgain.o player.o \
	strcasecmp.o strncasecmp.o tag.o tagger.o version.o  \
	file.o directory.o bstdfile.o rmalloc.o \
	audio_sjpcm.o isjpcm.o

EE_LIBS += -lfileXio -ldebug -lmad -lid3tag \
	-lcdvd -lsjpcm -lz -lm -lgcc -lmc -lc -lmc -lkernel

# excluded: bar.o option.o

EE_CXXFLAGS += -fno-exceptions -fno-rtti

EE_CFLAGS += -O2 -DHAVE_STRNCASECMP -DHAVE_CONFIG_H -DHAVE_UNISTD_H \
	-DHAVE_STRING_H -DHAVE_SYS_TYPES_H 

EE_INCS +=  -I../include -Iinclude -I$(PS2SDK)/common/include -I$(PS2SDK)/ee/include \
	-I$(PS2SDK)/ports/include -I$(PS2DEV)/isjpcm/include

EE_LDFLAGS += -L$(PS2SDK)/ee/lib -L$(PS2SDK)/ports/lib -L$(PS2DEV)/isjpcm/lib

all: $(EE_BIN)

install: $(EE_BIN)
	mkdir -p $(PS2SDK)/ports/bin
	cp $(EE_BIN) $(PS2SDK)/ports/bin/

isjpcm.s:
	bin2s $(PS2DEV)/isjpcm/bin/isjpcm.irx isjpcm.s isjpcm_irx

clean:
	rm -f *.elf *.o *.a *.s $(EE_BIN)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
