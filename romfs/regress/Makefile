
EE_BIN   = ps2tests.elf

EE_OBJS  = runner.o testsuite.o 
EE_OBJS += stdio_tests.o

EE_INCS = 


all: $(EE_BIN)

install:

clean:
	rm -f $(EE_BIN) $(EE_OBJS)

run:
	ps2client -h 192.168.0.100 execee host:$(EE_BIN)  
	
reset:
	ps2client -h 192.168.0.100 reset  

include Makefile.romfs
include Makefile.pref
include Makefile.eeglobal
