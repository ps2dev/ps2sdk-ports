/* This is the zlib uncompression stub for ps2-packer */

#include <kernel.h>
#include "packer-stub.h"
#include "zlib.h"

void Decompress(u8 *dest, const u8 *src, u32 dst_size, u32 src_size) {
    z_stream d_stream;
    
    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (voidpf)0;
    
    d_stream.next_in = src;
    d_stream.avail_in = src_size;
    d_stream.next_out = dest;
    d_stream.avail_out = dst_size;
    
    if (inflateInit(&d_stream) != Z_OK) {
#ifdef DEBUG
	printf("Error during inflateInit\n");
#endif
	SleepThread();
    }
    
    if (inflate(&d_stream, Z_NO_FLUSH) != Z_STREAM_END) {
#ifdef DEBUG
	printf("Error during inflate.\n");
#endif
	SleepThread();
    }
    
    if (inflateEnd(&d_stream) != Z_OK) {
#ifdef DEBUG
	printf("Error during inflateEnd.\n");
#endif
	SleepThread();
    }
}
