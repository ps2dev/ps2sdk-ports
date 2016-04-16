# Common rules shared by all build targets.

.PHONY: dummy

all: build

$(EE_LIB_DIR): dummy
	mkdir -p $(EE_LIB_DIR)

# Use SUBDIRS to descend into subdirectories.
subdir_list = $(patsubst %,all-%,$(SUBDIRS))
subdir_clean = $(patsubst %,clean-%,$(SUBDIRS))
subdir_install = $(patsubst %,install-%,$(SUBDIRS))
subdirs: dummy $(subdir_list)

build: $(EE_LIB_DIR) $(subdir_list)

install: build $(subdir_install)

clean: $(subdir_clean)

sample: build
	$(MAKE) -C $(SAMPLE_DIR)

ifdef SUBDIRS
$(subdir_list): dummy
	$(MAKE) -C $(patsubst all-%,%,$@)
$(subdir_clean): dummy
	$(MAKE) -C $(patsubst clean-%,%,$@) clean
	$(MAKE) -C $(SAMPLE_DIR) clean
	rm -f -r $(EE_LIB_DIR)
$(subdir_install): dummy
	$(MAKE) -C $(patsubst install-%,%,$@) install
endif

# Default rule for clean.
clean: $(subdir_clean)

# A rule to do nothing.
dummy:
