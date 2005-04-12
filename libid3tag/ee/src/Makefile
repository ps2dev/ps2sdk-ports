EE_LIB = ../lib/libid3tag.a
EE_OBJS = compat.o crc.o debug.o field.o file.o frame.o frametype.o \
	genre.o latin1.o parse.o render.o tag.o ucs4.o utf16.o utf8.o \
	util.o version.o

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal

install: all
	mkdir -p $(PS2SDK)/ports/include
	mkdir -p $(PS2SDK)/ports/lib
	cp -f $(EE_LIB) $(PS2SDK)/ports/lib
	cp -f ../include/id3tag.h $(PS2SDK)/ports/include

EE_CFLAGS += -DHAVE_CONFIG_H -DHAVE_SYS_TYPES_H -DHAVE_ERRNO_H \
	-DHAVE_UNISTD_H -DHAVE_FCNTL_H -I$(PS2SDK)/common/include/sys/ 

EE_INCS += -I$(PS2SDK)/ports/include -I../include
EE_LDFLAGS += -L$(PS2SDK)/ports/lib

all: $(EE_LIB)

clean:
	rm -f *.elf *.o $(EE_LIB)

