.PHONY: install

all:
	make -C src all

clean:
	make -C src clean

install:
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/include/stlport
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f lib/*.a $(DESTDIR)$(PS2SDK)/ports/lib
	cp -r stlport/* $(DESTDIR)$(PS2SDK)/ports/include/stlport
