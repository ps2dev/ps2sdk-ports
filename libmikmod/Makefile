# makefile for libmikmod
#------------------------------------------------------------------
# Review README & LICENSE files for further details.
#------------------------------------------------------------------
# Configuration flags :
#
# -- Enable FREESD instead of LIBSD
USE_FREESD=1
#------------------------------------------------------------------ 

EE_LIB_DIR = lib/

EE_LIBMIKMOD = $(EE_LIB_DIR)libmikmod.a

EE_LDFLAGS   += -L$(EE_LIB_DIR)
EE_INCS      += -I./include -I$(PS2SDK)/ports/include
EE_CFLAGS    += -DDRV_PS2 -DHAVE_FCNTL_H -DHAVE_MALLOC_H -DHAVE_UNISTD_H

ifeq ($(USE_FREESD), 1)
EE_CFLAGS += -DUSE_FREESD
endif

EE_OBJS  =  mmio/mmalloc.o mmio/mmerror.o 
#EE_OBJS +=  mmio/mmio.o

EE_OBJS +=  loaders/load_669.o loaders/load_amf.o loaders/load_dsm.o
EE_OBJS +=  loaders/load_far.o loaders/load_gdm.o loaders/load_imf.o
EE_OBJS +=  loaders/load_it.o loaders/load_m15.o loaders/load_med.o
EE_OBJS +=  loaders/load_mod.o loaders/load_mtm.o loaders/load_okt.o
EE_OBJS +=  loaders/load_s3m.o loaders/load_stm.o loaders/load_stx.o
EE_OBJS +=  loaders/load_ult.o loaders/load_uni.o loaders/load_xm.o

EE_OBJS +=  playercode/mdreg.o playercode/mdriver.o playercode/mdulaw.o
EE_OBJS +=  playercode/mloader.o playercode/mlreg.o playercode/mlutil.o
EE_OBJS +=  playercode/mplayer.o playercode/munitrk.o playercode/mwav.o
EE_OBJS +=  playercode/npertab.o playercode/sloader.o playercode/virtch2.o
EE_OBJS +=  playercode/virtch.o playercode/virtch_common.o

# PS2 specific code
EE_OBJS +=  mmio/mmio_ps2.o
EE_OBJS +=  drivers/drv_nos.o drivers/drv_ps2.o

# embedded objects
EE_OBJS += drivers/audsrv_irx.o 
ifeq ($(USE_FREESD), 1)
EE_OBJS += drivers/freesd_irx.o
endif

all: $(EE_LIB_DIR) $(EE_LIBMIKMOD)
	@echo ".LIBMIKMOD Built."
	@echo ".Use Makefile.sample to compile & run the sample."

sample: all
	make -f Makefile.sample

$(EE_LIB_DIR):
	mkdir -p $(EE_LIB_DIR)

EMBEDDED += \
        $(PS2SDK)/ee/lib/libaudsrv.a

$(EE_LIBMIKMOD): $(EE_OBJS)
	# packing with all embedded libraries
	rm -rf tmp
	mkdir tmp
	$(foreach f, $(EMBEDDED), (cd tmp; $(AR) x $f);)
	cp $(EE_OBJS) tmp
	$(EE_AR) cru $(EE_LIBMIKMOD) tmp/*
	rm -rf tmp

install: all
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/include
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/lib
	cp include/mikmod_build.h $(DESTDIR)$(PS2SDK)/ports/include/mikmod.h
	cp $(EE_LIBMIKMOD) $(DESTDIR)$(PS2SDK)/ports/lib

clean:
	rm -f -r $(EE_OBJS) $(EE_LIB_DIR) drivers/freesd_irx.s drivers/audsrv_irx.s
	make -f Makefile.sample clean

# inline

ifeq ($(USE_FREESD), 1)
drivers/freesd_irx.s: $(PS2SDK)/iop/irx/freesd.irx
	$(PS2SDK)/bin/bin2s $< $@ freesd_irx
endif
drivers/audsrv_irx.s : $(PS2SDK)/iop/irx/audsrv.irx
	$(PS2SDK)/bin/bin2s $< $@ audsrv_irx

include $(PS2SDK)/Defs.make
include ../Makefile.eeglobal
