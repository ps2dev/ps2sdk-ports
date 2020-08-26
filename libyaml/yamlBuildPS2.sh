echo '#include <sys/types.h>' >${PS2SDK}/ee/include/stdint.h
echo '#define PTRDIFF_MIN (-2147483647-1)' >>${PS2SDK}/ee/include/stdint.h
echo '#define PTRDIFF_MAX (2147483647)' >>${PS2SDK}/ee/include/stdint.h
sudo chown root ${PS2SDK}/ee/include/stdint.h

CC="ee-gcc" \
CFLAGS="-std=gnu99 -march=r5900 -mtune=r5900 -I${PS2SDK}/ee/include -I${PS2SDK}/common/include -I${PS2DEV}/ee/ee/sys-include" \
LDFLAGS="-L${PS2SDK}/ee/lib -T${PS2DEV}/ee/ee/lib/ldscripts/elf32l5900.x" \
LIBS="-lc" \
./configure --host=mips --prefix=${PS2SDK}/ee

make

sudo make install