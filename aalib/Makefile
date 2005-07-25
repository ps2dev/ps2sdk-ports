# ps2aalib
# aalib ps2port by brice rouanet
# tmator@gmail.com
# GPL V2

all :
	$(MAKE) -C src all
	$(MAKE) -C test all

clean :
	$(MAKE) -C src clean
	$(MAKE) -C test clean

install:
	cp -f include/* $(PS2SDK)/ports/include/
	cp -f lib/* $(PS2SDK)/ports/lib/
