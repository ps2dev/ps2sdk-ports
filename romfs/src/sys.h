/*
# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2005, ps2dev - http://www.ps2dev.org
# Licenced under GNU Library General Public License version 2
# Review ps2sdk README & LICENSE files for further details.
#
# ROMFS.
*/



#ifndef __SYS_H__
#define __SYS_H__


typedef unsigned char uint8;
typedef signed char int8;
typedef unsigned short uint16;
typedef signed short int16;
typedef unsigned int uint32;
typedef signed int int32;


#ifdef __cplusplus
extern "C" {
#endif


#if defined SYS_LITTLE_ENDIAN

inline uint16 READ_LE_UINT16(const void *ptr) {
#if defined SYS_NEED_ALIGNMENT
	/* gawd 20051002 */
	const uint8 *b = (const uint8 *)ptr;
	return (b[1] << 8) | b[0];
#else
	return *(const uint16 *)ptr;
#endif
}

inline uint32 READ_LE_UINT32(const void *ptr) {
#if defined SYS_NEED_ALIGNMENT
	/* gawd 20051002 */
	const uint8 *b = (const uint8 *)ptr;
	return (b[3] << 24) | (b[2] << 16) | (b[1] << 8) | b[0];
#else
	return *(const uint32 *)ptr;
#endif
}

inline uint16 READ_BE_UINT16(const void *ptr) {
	const uint8 *b = (const uint8 *)ptr;
	return (b[0] << 8) | b[1];
}

inline uint32 READ_BE_UINT32(const void *ptr) {
	const uint8 *b = (const uint8 *)ptr;
	return (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
}

#elif defined SYS_BIG_ENDIAN

inline uint16 READ_LE_UINT16(const void *ptr) {
	const uint8 *b = (const uint8 *)ptr;
	return (b[1] << 8) | b[0];
}

inline uint32 READ_LE_UINT32(const void *ptr) {
	const uint8 *b = (const uint8 *)ptr;
	return (b[3] << 24) | (b[2] << 16) | (b[1] << 8) | b[0];
}

inline uint16 READ_BE_UINT16(const void *ptr) {
#if defined SYS_NEED_ALIGNMENT
	/* gawd 20051002 */
	const uint8 *b = (const uint8 *)ptr;
	return (b[0] << 8) | b[1];
#else
	return *(const uint16 *)ptr;
#endif
}

inline uint32 READ_BE_UINT32(const void *ptr) {
#if defined SYS_NEED_ALIGNMENT
	/* gawd 20051002 */
	const uint8 *b = (const uint8 *)ptr;
	return (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
#else
	return *(const uint32 *)ptr;
#endif
}

#else

//#error No endianness defined

#endif

#ifdef __cplusplus
}
#endif


#endif // __SYS_H__
