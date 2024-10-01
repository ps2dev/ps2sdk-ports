EE_SRC_DIR = ../build/aalib-patched/aalib-1.4rc5/src/
# ps2aalib
# port basique de la aalib
# tmator@gmail.com
# GPL V2
#
EE_LIB_DIR = lib/

EE_LIB = $(EE_LIB_DIR)libaa.a
EE_INCS += -I$(EE_SRC_DIR)
EE_OBJS = $(EE_SRC_DIR)aafont.o \
       $(EE_SRC_DIR)aafonts.o \
       $(EE_SRC_DIR)aaattrs.o \
       $(EE_SRC_DIR)aacurrfnt.o \
       $(EE_SRC_DIR)aavyhen.o \
       $(EE_SRC_DIR)aaimage.o \
       $(EE_SRC_DIR)aaimgheight.o \
       $(EE_SRC_DIR)aaimgwidth.o \
       $(EE_SRC_DIR)aamktabl.o \
       $(EE_SRC_DIR)aakbdreg.o \
       $(EE_SRC_DIR)aamem.o \
       $(EE_SRC_DIR)aaprintf.o \
       $(EE_SRC_DIR)aahelp.o \
       $(EE_SRC_DIR)aaslnkbd.o \
       $(EE_SRC_DIR)aaslang.o \
       $(EE_SRC_DIR)aaputpixel.o \
       $(EE_SRC_DIR)aarecfunc.o \
       $(EE_SRC_DIR)aascrwidth.o \
       $(EE_SRC_DIR)aascrheight.o \
       $(EE_SRC_DIR)aafastre.o \
       $(EE_SRC_DIR)aatext.o \
       $(EE_SRC_DIR)fontgl.o \
       $(EE_SRC_DIR)font16.o \
       $(EE_SRC_DIR)font14.o \
       $(EE_SRC_DIR)fontx16.o \
       $(EE_SRC_DIR)fontx13.o \
       $(EE_SRC_DIR)fontx13b.o \
       $(EE_SRC_DIR)fontcour.o \
       $(EE_SRC_DIR)aarec.o \
       $(EE_SRC_DIR)font9.o \
       $(EE_SRC_DIR)font8.o \
       $(EE_SRC_DIR)fontline.o \
       $(EE_SRC_DIR)aalib.o \
       $(EE_SRC_DIR)aaflush.o \
       $(EE_SRC_DIR)aarender.o \
       $(EE_SRC_DIR)aaout.o \
       $(EE_SRC_DIR)aain.o \
       $(EE_SRC_DIR)aaedit.o \
       $(EE_SRC_DIR)aaregist.o \
       $(EE_SRC_DIR)aasave.o \
       $(EE_SRC_DIR)aastdin.o \
       aastdout.o \
       $(EE_SRC_DIR)aaparse.o

sample: all
	$(MAKE) -C test all

all: $(EE_LIB_DIR) $(EE_OBJS)
	$(EE_AR) rcs $(EE_LIB) $(EE_OBJS)

$(EE_LIB_DIR):
	mkdir -p $(EE_LIB_DIR)

install: all
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/include/
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/lib/
	cp -f *.h $(DESTDIR)$(PS2SDK)/ports/include/
	cp -f $(EE_LIB_DIR)* $(DESTDIR)$(PS2SDK)/ports/lib/

clean:
	rm -f -r $(EE_OBJS) $(EE_LIB_DIR)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
