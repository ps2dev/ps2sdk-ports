EE_OBJS_DIR = ../obj/
EE_LIB_DIR = ../lib/

EE_LIB = $(EE_LIB_DIR)libhash_library.a
EE_CXXFLAGS += -D__PS2__
EE_INCS += -I../include

EE_OBJS = crc32.o \
          keccak.o \
          md5.o \
          sha1.o \
          sha256.o \
          sha3.o

EE_OBJS := $(EE_OBJS:%=$(EE_OBJS_DIR)%)

PCFILE = hash-library.pc
PCFILE_IN = hash-library.pc.in

$(PCFILE): $(PCFILE_IN)
	sed \
	  -e 's|@prefix@|${PS2SDK}/ports|g' \
	  -e 's|@exec_prefix@|${PS2SDK}/ports|g' \
	  -e 's|@libdir@|${PS2SDK}/ports/lib|g' \
	  -e 's|@includedir@|${PS2SDK}/include|g' \
          $< >$@

all: $(EE_OBJS_DIR) $(EE_LIB_DIR) $(EE_OBJS) $(PCFILE)
	$(EE_AR) rcs $(EE_LIB) $(EE_OBJS)

$(EE_OBJS_DIR):
	mkdir -p $(EE_OBJS_DIR)

$(EE_LIB_DIR):
	mkdir -p $(EE_LIB_DIR)

$(EE_OBJS_DIR)%.o : %.cpp
	$(EE_CXX_COMPILE) -c $< -o $@

install: all
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/include/hash-library
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/lib
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/lib/pkgconfig
	cp -f ../include/* $(DESTDIR)$(PS2SDK)/ports/include/hash-library
	cp -f $(EE_LIB_DIR)* $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f  $(PCFILE) $(DESTDIR)$(PS2SDK)/ports/lib/pkgconfig

clean:
	rm -f -r $(EE_OBJS_DIR) $(EE_LIB_DIR)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
