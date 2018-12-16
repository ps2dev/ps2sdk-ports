EE_OBJS_DIR = lib/obj/GCC/
EE_LIB_DIR = lib/

all: $(EE_OBJS_DIR) $(EE_LIB_DIR)
	$(MAKE) -C src all

$(EE_OBJS_DIR):
	mkdir -p $(EE_OBJS_DIR)

$(EE_LIB_DIR):
	mkdir -p $(EE_LIB_DIR)

clean:
	rm -r -f $(EE_OBJS_DIR) $(EE_LIB_DIR)
	$(MAKE) -C src clean

install:
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/include/stlport
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f $(EE_LIB_DIR)*.a $(DESTDIR)$(PS2SDK)/ports/lib
	cp -r stlport/* $(DESTDIR)$(PS2SDK)/ports/include/stlport
