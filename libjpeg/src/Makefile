# some parts taken from myPS2's libjpg Makefile
EE_SRC_DIR = jpeg-6b/
EE_OBJS_DIR = ../obj/
EE_LIB_DIR = ../lib/

EE_LIB = $(EE_LIB_DIR)libjpeg.a

EE_CFLAGS =

SYSDEPMEM	= jmemnobs.o

# library object files common to compression and decompression
COMOBJECTS	=	jcomapi.o	\
			jutils.o	\
			jerror.o	\
			jmemmgr.o	\
			$(SYSDEPMEM)

# compression library object files
CLIBOBJECTS	=	jcapimin.o	\
			jcapistd.o	\
			jctrans.o	\
			jcparam.o	\
			jdatadst.o	\
			jcinit.o	\
			jcmaster.o	\
			jcmarker.o	\
			jcmainct.o	\
			jcprepct.o	\
			jccoefct.o	\
			jccolor.o	\
			jcsample.o	\
			jchuff.o	\
			jcphuff.o	\
			jcdctmgr.o	\
			jfdctfst.o	\
			jfdctflt.o	\
			jfdctint.o

# decompression library object files
DLIBOBJECTS	=	jdapimin.o	\
			jdapistd.o	\
			jdatasrc.o	\
			jdtrans.o	\
			jdmaster.o	\
			jdinput.o	\
			jdmarker.o	\
			jdhuff.o	\
			jdphuff.o	\
			jdmainct.o	\
			jdcoefct.o	\
			jdpostct.o	\
			jddctmgr.o	\
			jidctfst.o	\
			jidctflt.o	\
			jidctint.o	\
			jidctred.o	\
			jdsample.o	\
			jdcolor.o	\
			jquant1.o	\
			jquant2.o	\
			jdmerge.o

# ps2 jpg library object files
PS2OBJECTS = libjpg.o

EE_OBJS  = $(CLIBOBJECTS) $(DLIBOBJECTS) $(COMOBJECTS)
EE_OBJS := $(EE_OBJS:%=$(EE_OBJS_DIR)%)
EE_OBJS += $(PS2OBJECTS)

EE_INCS  = -I../include -I./$(EE_SRC_DIR)

all: $(EE_OBJS_DIR) $(EE_LIB_DIR) $(EE_LIB)

$(EE_OBJS_DIR):
	mkdir -p $(EE_OBJS_DIR)

$(EE_LIB_DIR):
	mkdir -p $(EE_LIB_DIR)

$(EE_OBJS_DIR)%.o : $(EE_SRC_DIR)%.c
	$(EE_C_COMPILE) -c $< -o $@

install: all
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/include
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f $(EE_LIB) $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f ../include/*.h $(DESTDIR)$(PS2SDK)/ports/include
	cp -f ../src/$(EE_SRC_DIR)jerror.h $(DESTDIR)$(PS2SDK)/ports/include

clean:
	rm -f -r $(EE_LIB_DIR) $(EE_OBJS_DIR) $(PS2OBJECTS)

sample:

include $(PS2SDK)/Defs.make
include ../../Makefile.eeglobal

