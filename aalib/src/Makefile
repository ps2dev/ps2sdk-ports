# ps2aalib
# port basique de la aalib
# tmator@gmail.com
# GPL V2
#
EE_OBJS_DIR = ../obj/
EE_LIB_DIR = ../lib/

EE_LIB = $(EE_LIB_DIR)libaa.a
EE_INCS += -I../include/
EE_OBJS = aafont.o \
       aafonts.o \
       aavyhen.o \
       aamktabl.o \
       aamem.o \
       aaprintf.o \
       aahelp.o \
       aaslnkbd.o \
       aaslang.o \
       aafastre.o \
       fontgl.o \
       font16.o \
       font14.o \
       fontx16.o \
       fontx13.o \
       fontx13b.o \
       fontcour.o \
       aarec.o \
       font9.o \
       font8.o \
       fontline.o \
       aalib.o \
       aaflush.o \
       aarender.o \
       aaout.o \
       aain.o \
       aaedit.o \
       aaregist.o \
       aasave.o \
       aastdout.o \
       aastdin.o \
       aaparse.o
EE_OBJS := $(EE_OBJS:%=$(EE_OBJS_DIR)%)

all: $(EE_OBJS_DIR) $(EE_LIB_DIR) $(EE_OBJS)
	$(EE_AR) rcs $(EE_LIB) $(EE_OBJS)

$(EE_OBJS_DIR):
	mkdir -p $(EE_OBJS_DIR)

$(EE_LIB_DIR):
	mkdir -p $(EE_LIB_DIR)

$(EE_OBJS_DIR)%.o : %.c
	$(EE_C_COMPILE) -c $< -o $@

install: all
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/include/
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/lib/
	cp -f ../include/* $(DESTDIR)$(PS2SDK)/ports/include/
	cp -f $(EE_LIB_DIR)* $(DESTDIR)$(PS2SDK)/ports/lib/

clean:
	rm -f -r $(EE_OBJS_DIR) $(EE_LIB_DIR)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
