EE_OBJS_DIR = ../obj/
EE_BIN_DIR = ../bin/

EE_BIN = $(EE_BIN_DIR)madplay.elf

EE_OBJS = audio.o audio_aiff.o audio_snd.o audio_wave.o audio_raw.o \
	audio_hex.o audio_cdda.o audio_null.o crc.o filter.o getopt.o \
	getopt1.o madplay.o resample.o rgain.o player.o \
	strcasecmp.o strncasecmp.o tag.o tagger.o version.o  \
	file.o directory.o bstdfile.o rmalloc.o \
	audio_sjpcm.o isjpcm.o
EE_OBJS := $(EE_OBJS:%=$(EE_OBJS_DIR)%)
# excluded: bar.o option.o

EE_LIBS += -lfileXio -ldebug -lmad -lid3tag \
	-lcdvd -lsjpcm -lz -lm -lgcc -lmc -lc -lmc -lkernel

EE_CXXFLAGS += -fno-exceptions -fno-rtti

EE_CFLAGS += -DHAVE_STRNCASECMP -DHAVE_CONFIG_H -DHAVE_UNISTD_H \
	-DHAVE_STRING_H -DHAVE_SYS_TYPES_H 

EE_INCS += -I./ -I../include -Iinclude -I$(PS2SDK)/common/include -I$(PS2SDK)/ee/include \
	-I$(PS2SDK)/ports/include -I$(PS2DEV)/isjpcm/include

EE_LDFLAGS += -L$(PS2SDK)/ee/lib -L$(PS2SDK)/ports/lib -L$(PS2DEV)/isjpcm/lib

all: $(EE_BIN_DIR) $(EE_OBJS_DIR) $(EE_BIN)

$(EE_BIN_DIR):
	mkdir -p $(EE_BIN_DIR)

$(EE_OBJS_DIR):
	mkdir -p $(EE_OBJS_DIR)

$(EE_OBJS_DIR)%.o : $(EE_SRC_DIR)%.c
	$(EE_C_COMPILE) -c $< -o $@

$(EE_OBJS_DIR)%.o : $(EE_SRC_DIR)%.s
	$(EE_AS) $(EE_ASFLAGS) $< -o $@

install: all
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/bin
	cp -f $(EE_BIN) $(DESTDIR)$(PS2SDK)/ports/bin/

sample: all

isjpcm.o:
	bin2o $(PS2DEV)/isjpcm/bin/isjpcm.irx isjpcm.c isjpcm_irx

clean:
	rm -f -r $(EE_OBJS_DIR) $(EE_BIN_DIR) isjpcm.c

include $(PS2SDK)/Defs.make
include ../../../Makefile.eeglobal
