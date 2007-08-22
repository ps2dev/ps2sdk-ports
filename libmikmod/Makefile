# makefile for libmikmod
#------------------------------------------------------------------
# Review README & LICENSE files for further details.
#------------------------------------------------------------------
# Configuration flags :
#
# -- Enable FREESD instead of LIBSD
USE_FREESD=1
#------------------------------------------------------------------ 



LIB_LIBMIKMOD = lib/libmikmod.a


EE_LDFLAGS   += -L./lib
EE_INCS      += -I./include -I$(PS2SDK)/ports/include
EE_CFLAGS    += -DDRV_PS2 -DHAVE_FCNTL_H -DHAVE_MALLOC_H -DHAVE_UNISTD_H


ifeq ($(USE_FREESD), 1)
EE_CFLAGS += -DUSE_FREESD
endif

LIBMIKMOD_OBJS  =  mmio/mmalloc.o mmio/mmerror.o 
#LIBMIKMOD_OBJS +=  mmio/mmio.o

LIBMIKMOD_OBJS +=  loaders/load_669.o loaders/load_amf.o loaders/load_dsm.o
LIBMIKMOD_OBJS +=  loaders/load_far.o loaders/load_gdm.o loaders/load_imf.o
LIBMIKMOD_OBJS +=  loaders/load_it.o loaders/load_m15.o loaders/load_med.o
LIBMIKMOD_OBJS +=  loaders/load_mod.o loaders/load_mtm.o loaders/load_okt.o
LIBMIKMOD_OBJS +=  loaders/load_s3m.o loaders/load_stm.o loaders/load_stx.o
LIBMIKMOD_OBJS +=  loaders/load_ult.o loaders/load_uni.o loaders/load_xm.o

LIBMIKMOD_OBJS +=  playercode/mdreg.o playercode/mdriver.o playercode/mdulaw.o
LIBMIKMOD_OBJS +=  playercode/mloader.o playercode/mlreg.o playercode/mlutil.o
LIBMIKMOD_OBJS +=  playercode/mplayer.o playercode/munitrk.o playercode/mwav.o
LIBMIKMOD_OBJS +=  playercode/npertab.o playercode/sloader.o playercode/virtch2.o
LIBMIKMOD_OBJS +=  playercode/virtch.o playercode/virtch_common.o

# PS2 specific code
LIBMIKMOD_OBJS +=  mmio/mmio_ps2.o
LIBMIKMOD_OBJS +=  drivers/drv_nos.o drivers/drv_ps2.o


# embedded objects
LIBMIKMOD_OBJS += drivers/audsrv_irx.o 
ifeq ($(USE_FREESD), 1)
LIBMIKMOD_OBJS += drivers/freesd_irx.o
endif

all: $(LIB_LIBMIKMOD) $(EE_BIN)
	@ECHO ".LIBMIKMOD Built."
	@ECHO ".Use Makefile.sample to compile & run the sample."

EMBEDDED += \
        $(PS2SDK)/ee/lib/libaudsrv.a


$(LIB_LIBMIKMOD): $(LIBMIKMOD_OBJS)
	# packing with all embedded libraries
	rm -rf tmp
	mkdir tmp
	$(foreach f, $(EMBEDDED), (cd tmp; $(AR) x $f);)
	cp $(LIBMIKMOD_OBJS) tmp
	$(EE_AR) cru $(LIB_LIBMIKMOD) tmp/*
	rm -rf tmp


install: all
	mkdir -p $(PS2SDK)/ports/include
	mkdir -p $(PS2SDK)/ports/lib
	cp include/mikmod_build.h $(PS2SDK)/ports/include/mikmod.h
	cp $(LIB_LIBMIKMOD) $(PS2SDK)/ports/lib


clean:
	/bin/rm -f $(LIBMIKMOD_OBJS) $(LIB_LIBMIKMOD)


# inline

ifeq ($(USE_FREESD), 1)
drivers/freesd_irx.o: $(PS2SDK)/iop/irx/freesd.irx
	$(PS2SDK)/bin/bin2o $< $@ freesd_irx
endif
drivers/audsrv_irx.o : $(PS2SDK)/iop/irx/audsrv.irx
	$(PS2SDK)/bin/bin2o $< $@ audsrv_irx


include Makefile.pref
include Makefile.eeglobal 

