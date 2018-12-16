EE_OBJS_DIR = obj/
EE_LIB_DIR = lib/

EE_OBJS = \
	tif_aux.o \
	tif_close.o \
	tif_codec.o \
	tif_color.o \
	tif_compress.o \
	tif_dir.o \
	tif_dirinfo.o \
	tif_dirread.o \
	tif_dirwrite.o \
	tif_dumpmode.o \
	tif_error.o \
	tif_extension.o \
	tif_fax3.o \
	tif_fax3sm.o \
	tif_flush.o \
	tif_getimage.o \
	tif_luv.o \
	tif_lzw.o \
	tif_open.o \
	tif_packbits.o \
	tif_predict.o \
	tif_print.o \
	tif_ps2.o \
	tif_read.o \
	tif_strip.o \
	tif_swab.o \
	tif_tile.o \
	tif_version.o \
	tif_warning.o \
	tif_write.o
EE_OBJS := $(EE_OBJS:%=$(EE_OBJS_DIR)%)

EE_CFLAGS += -msingle-float
EE_INCS += -I./
EE_LIB = $(EE_LIB_DIR)libtiff.a

all: $(EE_OBJS_DIR) $(EE_LIB_DIR) $(EE_LIB)

$(EE_OBJS_DIR):
	mkdir -p $(EE_OBJS_DIR)

$(EE_LIB_DIR):
	mkdir -p $(EE_LIB_DIR)

$(EE_OBJS_DIR)%.o : $(SOURCE_DIR)%.c
	$(EE_C_COMPILE) -c $< -o $@

clean:
	rm -f -r $(EE_OBJS_DIR) $(EE_LIB_DIR)

install: all
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/include/
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/lib/
	cp *.h $(DESTDIR)$(PS2SDK)/ports/include/
	cp $(EE_LIB) $(DESTDIR)$(PS2SDK)/ports/lib/

sample:

include $(PS2SDK)/Defs.make
include ../Makefile.eeglobal
