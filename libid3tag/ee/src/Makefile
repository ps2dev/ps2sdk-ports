EE_LIB = ../lib/libid3tag.a
EE_OBJS = compat.o crc.o debug.o field.o file.o frame.o frametype.o \
	genre.o latin1.o parse.o render.o tag.o ucs4.o utf16.o utf8.o \
	util.o version.o

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal

EE_CFLAGS += -DHAVE_CONFIG_H -DHAVE_SYS_TYPES_H -DHAVE_ERRNO_H \
	-DHAVE_UNISTD_H -DHAVE_FCNTL_H -I$(PS2SDK)/common/include/sys/ 

EE_INCS += -I$(ZLIB)/include -I../include
EE_LDFLAGS += -L$(ZLIB)/lib

all: $(EE_BIN)

clean:
	rm -f *.elf *.o $(EE_LIB)

