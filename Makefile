# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2001-2004, ps2dev - http://www.ps2dev.org
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.
#

SUBDIRS =
SUBDIRS += zlib
SUBDIRS += libpng
SUBDIRS += freetype2
SUBDIRS += libjpeg
SUBDIRS += libtiff
SUBDIRS += libmad
SUBDIRS += libid3tag
SUBDIRS += expat
SUBDIRS += libconfig-1.4.5
SUBDIRS += libmikmod
SUBDIRS += madplay
SUBDIRS += lua
SUBDIRS += sdl
SUBDIRS += sdlimage
SUBDIRS += sdlgfx
SUBDIRS += sdlmixer
SUBDIRS += sdlttf
SUBDIRS += aalib
# SUBDIRS += romfs #tools only works under linux
# SUBDIRS += ode #doesnt work
# SUBDIRS += stlport #doesnt work on 5.3.0
# SUBDIRS += ucl #doesnt work

all: $(patsubst %, _dir_%, $(SUBDIRS)) 

$(patsubst %, _dir_%, $(SUBDIRS)):
	@$(MAKE) -r -C $(patsubst _dir_%, %, $@)

clean: $(patsubst %, _cleandir_%, $(SUBDIRS))

$(patsubst %, _cleandir_%, $(SUBDIRS)):
	$(MAKE) -C $(patsubst _cleandir_%, %, $@) clean

install: $(patsubst %, _installdir_%, $(SUBDIRS))

$(patsubst %, _installdir_%, $(SUBDIRS)):
	$(MAKE) -C $(patsubst _installdir_%, %, $@) install
