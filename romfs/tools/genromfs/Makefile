
# Makefile for the genromfs program.

CFLAGS = -O2 -Wall #-g#
LDFLAGS = -s#-g

all: genromfs

genromfs: genromfs.o

clean:
	rm -f genromfs *.o

install: all install-bin install-man

install-bin:
	install -m 755 genromfs /usr/bin

install-man:
	install -m 644 genromfs.8 /usr/man/man8

