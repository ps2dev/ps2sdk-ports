EE_LIB = libfreetype.a
# SOURCE_DIR = ../orig/freetype/
SOURCE_DIR = orig/

all:
	cp -fr builds $(SOURCE_DIR)
	cd $(SOURCE_DIR) && make setup ps2
	cd $(SOURCE_DIR) && make

install: all
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/include
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/lib
	cp -fv $(SOURCE_DIR)/objs/$(EE_LIB) $(DESTDIR)$(PS2SDK)/ports/lib
	cp -frv $(SOURCE_DIR)/include/* $(DESTDIR)$(PS2SDK)/ports/include
	
sample:

clean:
	cd $(SOURCE_DIR) && make clean
	rm -f $(SOURCE_DIR)/objs/$(EE_LIB)
	rm -rf $(SOURCE_DIR)/builds/ps2
	rm -f $(SOURCE_DIR)/builds/compiler/ee-gcc.mk

include $(PS2SDK)/Defs.make
include ../Makefile.eeglobal
