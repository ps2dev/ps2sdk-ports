EE_SRC_DIR  = src/
EE_OBJS_DIR = obj/
EE_LIB_DIR  = lib/

EE_LIB      = $(EE_LIB_DIR)libromfs.a

EE_INCS    += -I./include

EE_CFLAGS  += -DSYS_LITTLE_ENDIAN -DSYS_NEED_ALIGNMENT

EE_OBJS    += romfs.o romfs_io.o
EE_OBJS    := $(EE_OBJS:%=$(EE_OBJS_DIR)%)

all: $(EE_OBJS_DIR) $(EE_LIB_DIR) $(EE_LIB)

tools: tools/genromfs
	$(MAKE) -C tools/genromfs all
	cp -f tools/genromfs/genromfs $(DESTDIR)$(PS2SDK)/bin/

$(EE_OBJS_DIR):
	mkdir -p $(EE_OBJS_DIR)

$(EE_LIB_DIR):
	mkdir -p $(EE_LIB_DIR)

$(EE_OBJS_DIR)%.o : $(EE_SRC_DIR)%.c
	$(EE_C_COMPILE) -c $< -o $@

install: all
	mkdir -p $(DESTDIR)$(PS2SDK)/bin/
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/include/
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f $(EE_LIB) $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f include/*.h $(DESTDIR)$(PS2SDK)/ports/include/

clean:
	rm -f -r $(EE_OBJS_DIR) $(EE_LIB_DIR)
	$(MAKE) -C regress/ clean
	$(MAKE) -C tools/genromfs clean

sample: all
	$(MAKE) -C regress/

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
