# Common rules shared by all build targets.

.PHONY: dummy

all: build

# Use SUBDIRS to descend into subdirectories.
subdir_list = $(patsubst %,all-%,$(SUBDIRS))
subdir_clean = $(patsubst %,clean-%,$(SUBDIRS))
subdir_install = $(patsubst %,install-%,$(SUBDIRS))
subdirs: dummy $(subdir_list)

build: $(subdir_list)

install: $(subdir_install)

clean: $(subdir_clean)

ifdef SUBDIRS
$(subdir_list): dummy
	$(MAKE) -C $(patsubst all-%,%,$@)
$(subdir_clean): dummy
	$(MAKE) -C $(patsubst clean-%,%,$@) clean
$(subdir_install): dummy
	$(MAKE) -C $(patsubst install-%,%,$@) install
endif

# Default rule for clean.
clean: $(subdir_clean)

# A rule to do nothing.
dummy:
