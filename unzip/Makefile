EE_SRC_DIR = ../build/zlib/contrib/minizip/
EE_OBJS_DIR = obj/
EE_LIB_DIR = lib/

EE_INCS += $(EE_SRC_DIR) -I$(PS2SDK)/ports/include
EE_LIBS += -lz

EE_LIB = $(EE_LIB_DIR)libunzip.a
EE_OBJS = unzip.o ioapi.o
EE_OBJS := $(EE_OBJS:%=$(EE_OBJS_DIR)%)

all: $(EE_OBJS_DIR) $(EE_LIB_DIR) $(EE_LIB)

$(EE_OBJS_DIR):
	mkdir -p $(EE_OBJS_DIR)

$(EE_LIB_DIR):
	mkdir -p $(EE_LIB_DIR)

$(EE_OBJS_DIR)%.o : $(EE_SRC_DIR)%.c
	$(EE_C_COMPILE) -DIOAPI_NO_64 -c $< -o $@

install: all
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/include
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f $(EE_LIB) $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f $(EE_SRC_DIR)unzip.h $(DESTDIR)$(PS2SDK)/ports/include
	cp -f $(EE_SRC_DIR)ioapi.h $(DESTDIR)$(PS2SDK)/ports/include

clean:
	rm -f -r $(EE_OBJS_DIR) $(EE_LIB_DIR)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal