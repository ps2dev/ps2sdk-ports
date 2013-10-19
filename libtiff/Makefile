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

EE_CFLAGS = -msingle-float
EE_INCS = -I./
EE_LIB = libtiff.a

all:	$(EE_LIB)

clean:
	rm -f $(EE_LIB) $(EE_OBJS)

install:
	cp *.h $(PS2SDK)/ports/include/
	cp $(EE_LIB) $(PS2SDK)/ports/lib/

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal

