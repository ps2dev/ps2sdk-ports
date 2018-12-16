EE_OBJS_DIR = obj/
EE_LIB_DIR = lib/

EE_LIB = $(EE_LIB_DIR)libz.a
SOURCE_DIR = src/

EE_INCS += -I$(SOURCE_DIR)

EE_OBJS = adler32.o compress.o crc32.o deflate.o gzclose.o gzlib.o gzread.o \
	gzwrite.o infback.o inffast.o inflate.o inftrees.o trees.o uncompr.o zutil.o
EE_OBJS := $(EE_OBJS:%=$(EE_OBJS_DIR)%)

TEST_OBJS = $(SOURCE_DIR)test/example.o
TEST_BIN = example.elf
TEST_CFLAGS = -I$(SOURCE_DIR)
TEST_LIBS = $(EE_LIB) -lc -lkernel
TEST_LDFLAGS += -mno-crt0
# TEST_LDFLAGS += -nostartfiles
TEST_LDFLAGS += -L$(PS2SDK)/ee/lib

all: $(EE_OBJS_DIR) $(EE_LIB_DIR) $(EE_LIB)

$(EE_OBJS_DIR):
	mkdir -p $(EE_OBJS_DIR)

$(EE_LIB_DIR):
	mkdir -p $(EE_LIB_DIR)

$(EE_OBJS_DIR)%.o : $(SOURCE_DIR)%.c
	$(EE_C_COMPILE) -c $< -o $@

$(TEST_BIN) : $(TEST_OBJS) $(PS2SDK)/ee/startup/crt0.o
	$(EE_CC) -T$(PS2SDK)/ee/startup/linkfile $(TEST_CFLAGS) \
		-o $(TEST_BIN) $(PS2SDK)/ee/startup/crt0.o $(TEST_OBJS) $(TEST_LDFLAGS) $(TEST_LIBS)
	gzip -f -c --best $(TEST_OBJS) > foo.gz

install: all
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/include
	mkdir -p $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f $(EE_LIB) $(DESTDIR)$(PS2SDK)/ports/lib
	cp -f $(SOURCE_DIR)zlib.h $(DESTDIR)$(PS2SDK)/ports/include
	cp -f $(SOURCE_DIR)zconf.h $(DESTDIR)$(PS2SDK)/ports/include

sample: all $(TEST_BIN)
	
clean:
	rm -f -r $(EE_OBJS_DIR) $(EE_LIB_DIR) $(TEST_OBJS) $(TEST_BIN) foo.gz

include $(PS2SDK)/Defs.make
include ../Makefile.eeglobal
