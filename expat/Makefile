EE_LIB_DIR = lib/

EE_LIB = $(EE_LIB_DIR)libexpat.a

EE_INCS = -I$(EE_LIB_DIR) -I$(PS2SDK)/common/include -I$(PS2SDK)/ee/include

EE_CFLAGS = -DHAVE_EXPAT_CONFIG_H

EE_OBJS = xmlparse.o xmlrole.o xmltok.o
EE_OBJS := $(EE_OBJS:%=$(EE_LIB_DIR)%)

all: $(EE_LIB)

install: all
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/include
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f $(EE_LIB) $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f $(EE_LIB_DIR)expat*.h $(DESTDIR)$(PS2SDK)/ports/include

clean:
	rm -f $(EE_OBJS) $(EE_LIB)

sample:

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
