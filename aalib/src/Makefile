# ps2aalib
# port basique de la aalib
# tmator@gmail.com
# GPL V2
#
LIBAA = ../lib/libaa.a
EE_INCS = -I../include/
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

all: $(EE_OBJS)
	$(EE_AR) rcs $(LIBAA) $(EE_OBJS)

clean:
	rm -f $(EE_OBJS)
	rm -f $(LIBAA)

include ../Makefile.pref
include ../Makefile.eeglobal

