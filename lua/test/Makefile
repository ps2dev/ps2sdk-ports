
PS2_IP = 192.168.0.100
TIMEOUT = 4

EE_BIN = mini_luap.elf

EE_OBJS = mini_luap.o

EE_LIBS   += -L$(PS2SDK)/ports/lib -llua -llualib -lm -lc -lmc -lkernel
EE_INCS   += -I../include -I$(PS2SDK)/ports/include

all: $(EE_BIN)

clean:
	rm -f $(EE_BIN) $(EE_OBJS)

tests: $(EE_BIN)
	ps2client -t $(TIMEOUT) -h $(PS2_IP) execee host:$(EE_BIN) bisect.lua
	ps2client -h $(PS2_IP) reset
	sleep 4
	ps2client -t $(TIMEOUT) -h $(PS2_IP) execee host:$(EE_BIN) cf.lua
	ps2client -h $(PS2_IP) reset
	sleep 4
	ps2client -t $(TIMEOUT) -h $(PS2_IP) execee host:$(EE_BIN) echo.lua
	ps2client -h $(PS2_IP) reset
	sleep 4
	ps2client -t $(TIMEOUT) -h $(PS2_IP) execee host:$(EE_BIN) factorial.lua
	ps2client -h $(PS2_IP) reset
	sleep 4
	ps2client -t $(TIMEOUT) -h $(PS2_IP) execee host:$(EE_BIN) fib.lua
	ps2client -h $(PS2_IP) reset
	sleep 4
	ps2client -t $(TIMEOUT) -h $(PS2_IP) execee host:$(EE_BIN) fibfor.lua
	ps2client -h $(PS2_IP) reset
	sleep 4
	ps2client -t $(TIMEOUT) -h $(PS2_IP) execee host:$(EE_BIN) globals.lua
	ps2client -h $(PS2_IP) reset
	sleep 4
	ps2client -t $(TIMEOUT) -h $(PS2_IP) execee host:$(EE_BIN) hello.lua
	ps2client -h $(PS2_IP) reset
	sleep 4
	ps2client -t $(TIMEOUT) -h $(PS2_IP) execee host:$(EE_BIN) life.lua
	ps2client -h $(PS2_IP) reset
	sleep 4
	ps2client -t $(TIMEOUT) -h $(PS2_IP) execee host:$(EE_BIN) printf.lua
	ps2client -h $(PS2_IP) reset
	sleep 4
	ps2client -t $(TIMEOUT) -h $(PS2_IP) execee host:$(EE_BIN) readonly.lua
	ps2client -h $(PS2_IP) reset
	sleep 4
	ps2client -t $(TIMEOUT) -h $(PS2_IP) execee host:$(EE_BIN) sieve.lua
	ps2client -h $(PS2_IP) reset
	sleep 4
	ps2client -t $(TIMEOUT) -h $(PS2_IP) execee host:$(EE_BIN) sort.lua
	ps2client -h $(PS2_IP) reset
	sleep 4
	ps2client -t $(TIMEOUT) -h $(PS2_IP) execee host:$(EE_BIN) table.lua
	ps2client -h $(PS2_IP) reset
	sleep 4
	ps2client -t $(TIMEOUT) -h $(PS2_IP) execee host:$(EE_BIN) test-export.lua
	ps2client -h $(PS2_IP) reset
	sleep 4
	ps2client -t $(TIMEOUT) -h $(PS2_IP) execee host:$(EE_BIN) test-error.lua
	ps2client -h $(PS2_IP) reset
	ps2client -t $(TIMEOUT) -h $(PS2_IP) execee host:$(EE_BIN) trace-calls.lua
	ps2client -h $(PS2_IP) reset
	sleep 4
	ps2client -t $(TIMEOUT) -h $(PS2_IP) execee host:$(EE_BIN) trace-globals.lua
	ps2client -h $(PS2_IP) reset
	sleep 4
	ps2client -t $(TIMEOUT) -h $(PS2_IP) execee host:$(EE_BIN) xd.lua
	ps2client -h $(PS2_IP) reset
	sleep 4

reset:
	ps2client -h $(PS2_IP) reset

include Makefile.pref
include Makefile.eeglobal