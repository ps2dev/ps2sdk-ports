ifneq ("$(wildcard $(PS2DEV)/isjpcm/include/sjpcm.h)","")
all:
	$(MAKE) -C ee/src all
else
all:
	@echo "Cannot find SjPCM library. Please install it into path from somewhere, for example from https://github.com/AKuHAK/isjpcm"
endif

clean:
	$(MAKE) -C ee/src clean

install: all
	$(MAKE) -C ee/src install

sample:
