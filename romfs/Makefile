
EE_LIB     = lib/libromfs.a

EE_INCS   += -I./include

EE_CFLAGS += -DSYS_LITTLE_ENDIAN -DSYS_NEED_ALIGNMENT

EE_OBJS   +=  src/romfs.o \
	      src/romfs_io.o


all: $(EE_LIB)
	$(MAKE) -C tools/genromfs all


install: all
	mkdir -p $(PS2SDK)/ports/include/
	mkdir -p $(PS2SDK)/ports/lib
	cp -f $(EE_LIB) $(PS2SDK)/ports/lib
	cp -f include/*.h $(PS2SDK)/ports/include/
	cp -f tools/genromfs/genromfs $(PS2SDK)/bin/

clean:
	rm -f $(EE_OBJS) $(EE_LIB)
	$(MAKE) -C tools/genromfs clean

include Makefile.pref
include Makefile.eeglobal 
