all:
	make -C src all

clean:
	make -C src clean

install:
	mkdir -p $(PS2SDK)/ports/include/stlport
	mkdir -p $(PS2SDK)/ports/lib
	cp -f lib/*.a $(PS2SDK)/ports/lib
	cp -r stlport/* $(PS2SDK)/ports/include/stlport
