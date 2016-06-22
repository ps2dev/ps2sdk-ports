##
# Makefile for PS2 port of libconfig
##

EE_SRC_DIR = src/
EE_OBJS_DIR = obj/
EE_LIB_DIR = lib/

EE_INCS += -I./include -I$(EE_SRC_DIR)include

EE_LIB = $(EE_LIB_DIR)libconfig.a
EE_OBJS = grammar.o libconfig.o scanctx.o scanner.o strbuf.o
EE_OBJS := $(EE_OBJS:%=$(EE_OBJS_DIR)%)

all: $(EE_OBJS_DIR) $(EE_LIB_DIR) $(EE_LIB)

$(EE_OBJS_DIR):
	mkdir -p $(EE_OBJS_DIR)

$(EE_LIB_DIR):
	mkdir -p $(EE_LIB_DIR)

$(EE_OBJS_DIR)%.o : $(EE_SRC_DIR)%.c
	$(EE_C_COMPILE) -c $< -o $@

install: all
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/include
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f $(EE_LIB) $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f include/*.h $(DESTDIR)$(PS2SDK)/ports/include

clean:
	rm -f -r $(EE_OBJS_DIR) $(EE_LIB_DIR)

sample:

include $(PS2SDK)/Defs.make
include ../Makefile.eeglobal
