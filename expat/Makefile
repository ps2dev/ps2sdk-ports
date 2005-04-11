EE_LIB = lib/libexpat.a

EE_INCS = -Ilib -I$(PS2SDK)/common/include -I$(PS2SDK)/ee/include

EE_CFLAGS = -DHAVE_EXPAT_CONFIG_H

EE_OBJS = lib/xmlparse.o lib/xmlrole.o lib/xmltok.o

all: $(EE_LIB)

install: all
	mkdir -p $(PS2SDK)/ports/include
	mkdir -p $(PS2SDK)/ports/lib
	cp -f $(EE_LIB) $(PS2SDK)/ports/lib
	cp -f lib/expat*.h $(PS2SDK)/ports/include

clean:
	/bin/rm -f $(EE_OBJS) $(EE_LIB)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal 
