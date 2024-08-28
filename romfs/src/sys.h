/*
# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2005-Present, ps2dev - http://www.ps2dev.org
# Licenced under GNU Library General Public License version 2
# Review ps2sdk README & LICENSE files for further details.
#
# ROMFS.
*/

#ifndef __SYS_H__
#define __SYS_H__

#include <tamtypes.h>

#ifdef __cplusplus
extern "C" {
#endif

inline u16 READ_LE_UINT16(const void *ptr) {
#if defined SYS_NEED_ALIGNMENT
	/* gawd 20051002 */
	const u8 *b = (const u8 *)ptr;
	return (b[1] << 8) | b[0];
#else
	return *(const u16 *)ptr;
#endif
}

inline u32 READ_LE_UINT32(const void *ptr) {
#if defined SYS_NEED_ALIGNMENT
	/* gawd 20051002 */
	const u8 *b = (const u8 *)ptr;
	return (b[3] << 24) | (b[2] << 16) | (b[1] << 8) | b[0];
#else
	return *(const u32 *)ptr;
#endif
}

inline u16 READ_BE_UINT16(const void *ptr) {
#ifdef SYS_BIG_ENDIAN
	return *(const u16 *)ptr;
#else
	const u8 *b = (const u8 *)ptr;
	return (b[0] << 8) | b[1];
#endif
}

inline u32 READ_BE_UINT32(const void *ptr) {
#ifdef SYS_BIG_ENDIAN
	return *(const u32 *)ptr;
#else
	const u8 *b = (const u8 *)ptr;
	return (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
#endif
}

#ifdef __cplusplus
}
#endif


#endif // __SYS_H__
