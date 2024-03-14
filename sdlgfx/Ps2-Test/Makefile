ifneq ("$(wildcard $(GSKIT)/include/gsKit.h)","")
all :
	$(MAKE) -C  imagefilter all
	$(MAKE) -C  fonts all
	$(MAKE) -C  rotozoom all
	$(MAKE) -C  framerate all
	$(MAKE) -C  gfxprimitives all
else
all:
	@echo "GSKIT not set and gsKit not installed"
endif

clean :
	$(MAKE) -C  imagefilter clean
	$(MAKE) -C  fonts clean
	$(MAKE) -C  rotozoom clean
	$(MAKE) -C  framerate clean
	$(MAKE) -C  gfxprimitives clean
