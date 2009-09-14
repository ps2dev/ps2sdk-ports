# some parts taken from myPS2's libjpg Makefile

EE_LIB=../lib/libjpeg.a

EE_CFLAGS =

SYSDEPMEM	= jpeg-6b/jmemnobs.o

# library object files common to compression and decompression
COMOBJECTS	=	jpeg-6b/jcomapi.o	\
			jpeg-6b/jutils.o	\
			jpeg-6b/jerror.o	\
			jpeg-6b/jmemmgr.o	\
			$(SYSDEPMEM)

# compression library object files
CLIBOBJECTS	=	jpeg-6b/jcapimin.o	\
			jpeg-6b/jcapistd.o	\
			jpeg-6b/jctrans.o	\
			jpeg-6b/jcparam.o	\
			jpeg-6b/jdatadst.o	\
			jpeg-6b/jcinit.o	\
			jpeg-6b/jcmaster.o	\
			jpeg-6b/jcmarker.o	\
			jpeg-6b/jcmainct.o	\
			jpeg-6b/jcprepct.o	\
			jpeg-6b/jccoefct.o	\
			jpeg-6b/jccolor.o	\
			jpeg-6b/jcsample.o	\
			jpeg-6b/jchuff.o	\
			jpeg-6b/jcphuff.o	\
			jpeg-6b/jcdctmgr.o	\
			jpeg-6b/jfdctfst.o	\
			jpeg-6b/jfdctflt.o	\
			jpeg-6b/jfdctint.o

# decompression library object files
DLIBOBJECTS	=	jpeg-6b/jdapimin.o	\
			jpeg-6b/jdapistd.o	\
			jpeg-6b/jdatasrc.o	\
			jpeg-6b/jdtrans.o	\
			jpeg-6b/jdmaster.o	\
			jpeg-6b/jdinput.o	\
			jpeg-6b/jdmarker.o	\
			jpeg-6b/jdhuff.o	\
			jpeg-6b/jdphuff.o	\
			jpeg-6b/jdmainct.o	\
			jpeg-6b/jdcoefct.o	\
			jpeg-6b/jdpostct.o	\
			jpeg-6b/jddctmgr.o	\
			jpeg-6b/jidctfst.o	\
			jpeg-6b/jidctflt.o	\
			jpeg-6b/jidctint.o	\
			jpeg-6b/jidctred.o	\
			jpeg-6b/jdsample.o	\
			jpeg-6b/jdcolor.o	\
			jpeg-6b/jquant1.o	\
			jpeg-6b/jquant2.o	\
			jpeg-6b/jdmerge.o

# ps2 jpg library object files
PS2OBJECTS = libjpg.o

EE_OBJS = $(CLIBOBJECTS) $(DLIBOBJECTS) $(COMOBJECTS) $(PS2OBJECTS)

EE_INCS  = -I../include -I./jpeg-6b

all: $(EE_LIB)

install: all
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/include
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f ../lib/$(EE_LIB) $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f ../include/*.h $(DESTDIR)$(PS2SDK)/ports/include
	cp -f ../src/jpeg-6b/jerror.h $(DESTDIR)$(PS2SDK)/ports/include

clean:
	rm -f $(EE_LIB) $(EE_OBJS)

include ../Makefile.pref
include ../Makefile.eeglobal

