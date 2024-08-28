# ps2aalib
# port basique de la aalib
# tmator@gmail.com
# GPL V2
#
EE_SRC_DIR = ../../build/aalib-patched/aalib-1.4rc5/src/
EE_LIB_DIR = ../lib/

EE_INCS += -I$(EE_SRC_DIR) -I$(PS2SDK)/ee/include
EE_BIN = aafire.elf
EE_OBJS = $(EE_SRC_DIR)aafire.o
EE_LDFLAGS = -L$(EE_LIB_DIR) -L$(PS2SDK)/ee/lib
EE_LIBS = $(EE_LIB_DIR)libaa.a -ldebug -lm

all: $(EE_BIN)

clean:
	rm -f $(EE_BIN) $(EE_OBJS)

include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal
