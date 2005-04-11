# makefile for sdlimage.a 
#------------------------------------------------------------------
# Review README & LICENSE files for further details.
#------------------------------------------------------------------
# GCC 3.2.2 / PS2SDK 1.2 / GSKIT / SDL / ZLIB / PNG / JPG
#------------------------------------------------------------------
# Compilation Flags info
# -DLOAD_TIF not supported (no ps2 tiff lib available)
#------------------------------------------------------------------ 

EE_LIB = lib/libSDL_image.a

EE_INCS      += -I./include -I$(PS2SDK)/ports/include -I$(PS2SDK)/ports/include/SDL
EE_LIBS      += -lpng -lz -ljpg -lSDL

EE_CFLAGS    += -DLOAD_BMP \
		-DLOAD_GIF \
		-DLOAD_JPG \
		-DLOAD_LBM \
		-DLOAD_PCX \
		-DLOAD_PNG \
		-DLOAD_PNM \
		-DLOAD_TGA \
		-DLOAD_XCF \
		-DLOAD_XPM

EE_OBJS =	IMG.o			\
		IMG_bmp.o		\
		IMG_gif.o		\
		IMG_jpg.o		\
		IMG_lbm.o		\
		IMG_pcx.o		\
		IMG_png.o		\
		IMG_pnm.o		\
		IMG_tga.o		\
		IMG_tif.o		\
		IMG_xcf.o		\
		IMG_xpm.o

all: $(EE_LIB)

install: all
	mkdir -p $(PS2SDK)/ports/include/SDL
	mkdir -p $(PS2SDK)/ports/lib
	cp -f $(EE_LIB) $(PS2SDK)/ports/lib
	cp -f include/*.h $(PS2SDK)/ports/include/SDL

clean:
	/bin/rm -f $(EE_OBJS) $(EE_LIB)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal 

