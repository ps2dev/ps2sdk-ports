EE_OBJS_DIR = obj/
EE_LIB_DIR = lib/

EE_OBJS = alloc.o n2b_99.o n2b_d.o n2b_ds.o n2b_to.o n2d_99.o n2d_d.o n2d_ds.o \
    n2d_to.o n2e_99.o n2e_d.o n2e_ds.o n2e_to.o ucl_crc.o ucl_init.o ucl_ptr.o \
    ucl_str.o ucl_util.o 
EE_OBJS := $(EE_OBJS:%=$(EE_OBJS_DIR)%)

EE_LIB = $(EE_LIB_DIR)libucl.a

all: $(EE_OBJS_DIR) $(EE_LIB_DIR) $(EE_LIB)

clean:
	rm -f -r $(EE_OBJS_DIR) $(EE_LIB_DIR)

$(EE_OBJS_DIR):
	mkdir -p $(EE_OBJS_DIR)

$(EE_LIB_DIR):
	mkdir -p $(EE_LIB_DIR)

$(EE_OBJS_DIR)%.o : %.c
	$(EE_C_COMPILE) -c $< -o $@
	
sample: all
	$(MAKE) -C test

install: all
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/include/ucl
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f $(EE_LIB) $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f ucl/*.h $(DESTDIR)$(PS2SDK)/ports/include/ucl

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
