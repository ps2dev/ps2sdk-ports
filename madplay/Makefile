ifneq ("$(wildcard $(PS2DEV)/isjpcm/include/sjpcm.h)","")
all:
	$(MAKE) -C ee/src all

install: all
	$(MAKE) -C ee/src install

sample:

else
all:
	@echo "Cannot find SjPCM library. Please install it into path from somewhere, for example from https://github.com/AKuHAK/isjpcm"

install: all

sample:

endif

clean:
	$(MAKE) -C ee/src clean
