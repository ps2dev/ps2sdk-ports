include $(PS2SDK)/Defs.make

EE_OBJS = alloc.o n2b_99.o n2b_d.o n2b_ds.o n2b_to.o n2d_99.o n2d_d.o n2d_ds.o \
    n2d_to.o n2e_99.o n2e_d.o n2e_ds.o n2e_to.o ucl_crc.o ucl_init.o ucl_ptr.o \
    ucl_str.o ucl_util.o 

EE_CFLAGS = -D_EE -O3
EE_INC = -I. -I $(PS2SDK)/common/include -I $(PS2SDK)/ee/include

EE_LIB = libucl.a

all: $(EE_LIB)

$(EE_LIB): $(EE_OBJS)
	$(EE_AR) rc $(EE_LIB) $(EE_OBJS)

clean:
	rm -f $(EE_OBJS) $(EE_LIB)

install: all
	mkdir -p $(PS2SDK)/ports/include/ucl
	mkdir -p $(PS2SDK)/ports/lib
	cp -f $(EE_LIB) $(PS2SDK)/ports/lib
	cp -f ucl/*.h $(PS2SDK)/ports/include/ucl

.c.o :
	$(EE_CC) $(EE_CFLAGS) $(EE_INC) -c $< -o $@
