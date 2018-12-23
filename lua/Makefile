EE_SRC_DIR = src/
EE_LIB_DIR = lib/

EE_LIBLUA 	= $(EE_LIB_DIR)liblua.a
EXTRA_TARGETS 	= $(EE_LIB_DIR)liblualib.a

EE_CFLAGS 	+= -fomit-frame-pointer -fno-exceptions #-fno-rtti
# EE_CFLAGS 	+= -DCLOCKS_PER_SEC=1000 

EE_INCS 	+= -I$(EE_SRC_DIR)

LIBLUA_OBJS 	= lapi.o lcode.o ldebug.o ldo.o ldump.o lfunc.o \
		lgc.o llex.o lmem.o lobject.o lopcodes.o \
		lparser.o lstate.o lstring.o ltable.o \
		ltm.o lundump.o lvm.o lzio.o ps2_stub.o
LIBLUA_OBJS := $(LIBLUA_OBJS:%=$(EE_SRC_DIR)%)

LIBLUALIB_OBJS 	= lauxlib.o lbaselib.o ldblib.o \
		liolib.o lmathlibf.o loslib.o ltablib.o \
		lstrlib.o loadlib.o linit.o
LIBLUALIB_OBJS := $(LIBLUALIB_OBJS:%=$(EE_SRC_DIR)%)

all: $(EE_LIB_DIR) $(EE_LIBLUA) $(EXTRA_TARGETS)

$(EE_LIB_DIR):
	mkdir -p $(EE_LIB_DIR)

$(EE_LIBLUA): $(LIBLUA_OBJS)
	$(EE_AR) cru $@ $^

$(EXTRA_TARGETS): $(LIBLUALIB_OBJS)
	$(EE_AR) cru $@ $^

sample: all
	$(MAKE) -C test

clean:
	rm -f -r $(LIBLUA_OBJS) $(LIBLUALIB_OBJS) $(EE_LIB_DIR)
	$(MAKE) -C test clean

install: all
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/include
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/lib
	cp $(EE_SRC_DIR)lua.h $(EE_SRC_DIR)luaconf.h $(EE_SRC_DIR)lualib.h $(EE_SRC_DIR)lauxlib.h etc/lua.hpp $(DESTDIR)$(PS2SDK)/ports/include
	cp $(EE_LIB_DIR)*.a $(DESTDIR)$(PS2SDK)/ports/lib


# DO NOT DELETE

lapi.o: lapi.c lua.h luaconf.h lapi.h lobject.h llimits.h ldebug.h \
  lstate.h ltm.h lzio.h lmem.h ldo.h lfunc.h lgc.h lstring.h ltable.h \
  lundump.h lvm.h
lauxlib.o: lauxlib.c lua.h luaconf.h lauxlib.h
lbaselib.o: lbaselib.c lua.h luaconf.h lauxlib.h lualib.h
lcode.o: lcode.c lua.h luaconf.h lcode.h llex.h lobject.h llimits.h \
  lzio.h lmem.h lopcodes.h lparser.h ltable.h ldebug.h lstate.h ltm.h \
  ldo.h lgc.h
ldblib.o: ldblib.c lua.h luaconf.h lauxlib.h lualib.h
ldebug.o: ldebug.c lua.h luaconf.h lapi.h lobject.h llimits.h lcode.h \
  llex.h lzio.h lmem.h lopcodes.h lparser.h ltable.h ldebug.h lstate.h \
  ltm.h ldo.h lfunc.h lstring.h lgc.h lvm.h
ldo.o: ldo.c lua.h luaconf.h ldebug.h lstate.h lobject.h llimits.h ltm.h \
  lzio.h lmem.h ldo.h lfunc.h lgc.h lopcodes.h lparser.h ltable.h \
  lstring.h lundump.h lvm.h
ldump.o: ldump.c lua.h luaconf.h lobject.h llimits.h lstate.h ltm.h \
  lzio.h lmem.h lundump.h
lfunc.o: lfunc.c lua.h luaconf.h lfunc.h lobject.h llimits.h lgc.h lmem.h \
  lstate.h ltm.h lzio.h
lgc.o: lgc.c lua.h luaconf.h ldebug.h lstate.h lobject.h llimits.h ltm.h \
  lzio.h lmem.h ldo.h lfunc.h lgc.h lstring.h ltable.h
linit.o: linit.c lua.h luaconf.h lualib.h lauxlib.h
liolib.o: liolib.c lua.h luaconf.h lauxlib.h lualib.h
llex.o: llex.c lua.h luaconf.h ldo.h lobject.h llimits.h lstate.h ltm.h \
  lzio.h lmem.h llex.h lparser.h ltable.h lstring.h lgc.h
lmathlib.o: lmathlib.c lua.h luaconf.h lauxlib.h lualib.h
lmem.o: lmem.c lua.h luaconf.h ldebug.h lstate.h lobject.h llimits.h \
  ltm.h lzio.h lmem.h ldo.h
loadlib.o: loadlib.c lauxlib.h lua.h luaconf.h lobject.h llimits.h \
  lualib.h
lobject.o: lobject.c lua.h luaconf.h ldo.h lobject.h llimits.h lstate.h \
  ltm.h lzio.h lmem.h lstring.h lgc.h lvm.h
lopcodes.o: lopcodes.c lopcodes.h llimits.h lua.h luaconf.h
loslib.o: loslib.c lua.h luaconf.h lauxlib.h lualib.h
lparser.o: lparser.c lua.h luaconf.h lcode.h llex.h lobject.h llimits.h \
  lzio.h lmem.h lopcodes.h lparser.h ltable.h ldebug.h lstate.h ltm.h \
  ldo.h lfunc.h lstring.h lgc.h
lstate.o: lstate.c lua.h luaconf.h ldebug.h lstate.h lobject.h llimits.h \
  ltm.h lzio.h lmem.h ldo.h lfunc.h lgc.h llex.h lstring.h ltable.h
lstring.o: lstring.c lua.h luaconf.h lmem.h llimits.h lobject.h lstate.h \
  ltm.h lzio.h lstring.h lgc.h
lstrlib.o: lstrlib.c lua.h luaconf.h lauxlib.h lualib.h
ltable.o: ltable.c lua.h luaconf.h ldebug.h lstate.h lobject.h llimits.h \
  ltm.h lzio.h lmem.h ldo.h lgc.h ltable.h
ltablib.o: ltablib.c lua.h luaconf.h lauxlib.h lualib.h
ltm.o: ltm.c lua.h luaconf.h lobject.h llimits.h lstate.h ltm.h lzio.h \
  lmem.h lstring.h lgc.h ltable.h
lua.o: lua.c lua.h luaconf.h lauxlib.h lualib.h
luac.o: luac.c lua.h luaconf.h lauxlib.h ldo.h lobject.h llimits.h \
  lstate.h ltm.h lzio.h lmem.h lfunc.h lopcodes.h lstring.h lgc.h \
  lundump.h
lundump.o: lundump.c lua.h luaconf.h ldebug.h lstate.h lobject.h \
  llimits.h ltm.h lzio.h lmem.h ldo.h lfunc.h lstring.h lgc.h lundump.h
lvm.o: lvm.c lua.h luaconf.h ldebug.h lstate.h lobject.h llimits.h ltm.h \
  lzio.h lmem.h ldo.h lfunc.h lgc.h lopcodes.h lstring.h ltable.h lvm.h
lzio.o: lzio.c lua.h luaconf.h llimits.h lmem.h lstate.h lobject.h ltm.h \
  lzio.h
print.o: print.c ldebug.h lstate.h lua.h luaconf.h lobject.h llimits.h \
  ltm.h lzio.h lmem.h lopcodes.h lundump.h

# (end of Makefile)

include $(PS2SDK)/Defs.make
include ../Makefile.eeglobal
