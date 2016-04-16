#
# FreeType 2 configuration file to detect an PlayStation2 host platform.
# Modified by Nic, further modified by ooPo.
# (This is just a hack to make it work)
#

# Copyright 1996-2000, 2003, 2006 by
# David Turner, Robert Wilhelm, and Werner Lemberg.
#
# This file is part of the FreeType project, and may only be used, modified,
# and distributed under the terms of the FreeType project license,
# LICENSE.TXT.  By continuing to use, modify, or distribute this file you
# indicate that you have read the license and understand and accept it
# fully.


.PHONY: setup


ifeq ($(PLATFORM),ansi)

  ifdef PS2_HOST_CPU

    PLATFORM := ps2

  endif # test MACHTYPE ps2
endif

ifeq ($(PLATFORM),ps2)

  TOP_DIR     := .
  DELETE      := rm -f
  SEP         := /
  BUILD_DIR   := $(TOP_DIR)/builds/ps2
  CONFIG_FILE := ps2.mk
  CC          := $(EE_CC)

  setup: std_setup

endif # test PLATFORM ps2


# EOF
