#ifndef __PACKER_STUB_H__
#define __PACKER_STUB_H__

#include <tamtypes.h>

void Decompress(u8 *dest, const u8 *src, u32 dst_size, u32 src_size);

/*
  The data stream, located at the pointer PackedELF is created that way:
  | packed_Header | packed_SectionHeader | compressed data | packed_SectionHeader | compressed data | ...
*/
     
typedef struct {
    u32 entryAddr;
    u32 numSections;
} packed_Header;

typedef struct {
    u32 originalSize;
    u32 zeroByteSize;
    u32 virtualAddr;
    u32 compressedSize;
} packed_SectionHeader;

#endif
