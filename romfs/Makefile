
EE_LIB     = lib/libromfs.a

EE_INCS   += -I./include

EE_CFLAGS += -DSYS_LITTLE_ENDIAN -DSYS_NEED_ALIGNMENT

EE_OBJS   +=  src/romfs.o \
	      src/romfs_io.o

all: create_lib $(EE_LIB)
	$(MAKE) -C tools/genromfs all

create_lib:
	mkdir -p lib

install: all
	mkdir -p $(DESTDIR)$(PS2SDK)/bin/
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/include/
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f $(EE_LIB) $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f include/*.h $(DESTDIR)$(PS2SDK)/ports/include/
	cp -f tools/genromfs/genromfs $(DESTDIR)$(PS2SDK)/bin/

clean:
	rm -f $(EE_OBJS) $(EE_LIB)
	$(MAKE) -C tools/genromfs clean

include $(PS2SDK)/Defs.make
include ../Makefile.eeglobal
