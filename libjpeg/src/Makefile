EE_LIB=../lib/libjpeg.a

EE_CFLAGS = 

EE_OBJS = jcapimin.o jcapistd.o jccoefct.o jccolor.o jcdctmgr.o jchuff.o \
	jcinit.o jcmainct.o jcmarker.o jcmaster.o jcomapi.o jcparam.o \
        jcphuff.o jcprepct.o jcsample.o jctrans.o jdapimin.o jdapistd.o \
        jdatadst.o jdatasrc.o jdcoefct.o jdcolor.o jddctmgr.o jdhuff.o \
        jdinput.o jdmainct.o jdmarker.o jdmaster.o jdmerge.o jdphuff.o \
        jdpostct.o jdsample.o jdtrans.o jerror.o jfdctflt.o \
        jidctflt.o jidctred.o jquant1.o \
        jquant2.o jutils.o jmemmgr.o jmemnobs.o

EE_INCS  = -I../include

all: $(EE_LIB)

install: all
	mkdir -p $(PS2SDK)/ports/include
	mkdir -p $(PS2SDK)/ports/lib
	cp -f ../lib/$(EE_LIB) $(PS2SDK)/ports/lib
	cp -f ../include/*.h $(PS2SDK)/ports/include

clean:
	rm -f $(EE_LIB) $(EE_OBJS)

include ../Makefile.pref
include ../Makefile.eeglobal

