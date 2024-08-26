EE_OBJS_DIR = ../obj/
EE_LIB_DIR = ../lib/

EE_LIB = $(EE_LIB_DIR)libtiff_ps2_addons.a

EE_CFLAGS =

# ps2 tiff addons object files
PS2OBJECTS = libtiff_ps2_addons.o

EE_OBJS += $(PS2OBJECTS)

EE_INCS  = -I../include -I./$(EE_SRC_DIR)
EE_INCS += -I$(PS2SDK)/ports/include -I../include
EE_LDFLAGS += -L$(PS2SDK)/ports/lib

all: $(EE_OBJS_DIR) $(EE_LIB_DIR) $(EE_LIB)

$(EE_OBJS_DIR):
	mkdir -p $(EE_OBJS_DIR)

$(EE_LIB_DIR):
	mkdir -p $(EE_LIB_DIR)

$(EE_OBJS_DIR)%.o : %.c
	$(EE_C_COMPILE) -c $< -o $@

install: all
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/include
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f $(EE_LIB) $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f ../include/*.h $(DESTDIR)$(PS2SDK)/ports/include

clean:
	rm -f -r $(EE_LIB_DIR) $(EE_OBJS_DIR) $(PS2OBJECTS)

sample:

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal

