all: ucl-test.elf

ucl-test.elf: ucl-test.c n2e.S
	ee-gcc -D_EE -o ucl-test.elf ucl-test.c n2e.S -I ../ -L ../ -L $(PS2SDK)/ee/lib -I $(PS2SDK)/ee/include -I $(PS2SDK)/common/include -lucl -lc -lkernel -T $(PS2SDK)/ee/startup/linkfile

clean:
	rm -f ucl-test.elf
