.PHONY: install

all:
	$(MAKE) -C src all

clean:
	$(MAKE) -C src clean

install:
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/include/stlport
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f lib/*.a $(DESTDIR)$(PS2SDK)/ports/lib
	cp -r stlport/* $(DESTDIR)$(PS2SDK)/ports/include/stlport
