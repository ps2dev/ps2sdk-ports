#ifndef __LIBJPG_H__
#define __LIBJPG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

// JPG_WIDTH_FIX ensures Width is a multiple of four.
//
#define JPG_NORMAL 0x00
#define JPG_WIDTH_FIX 0x01

typedef struct {
	int width;
	int height;
	int bpp;
	void *buffer;
} jpgData;

jpgData *jpgFromRAW(void *data, int size, int mode);
jpgData *jpgFromFilename(const char *filename, int mode);
jpgData *jpgFromFILE(FILE *in_file, int mode);
void jpgFileFromJpgData(const char *filename, int quality, jpgData *jpg);
int jpgScreenshot(const char* pFilename,unsigned int VramAdress, unsigned int Width, unsigned int Height, unsigned int Psm);

#ifdef __cplusplus
}
#endif

#endif /* __LIBJPG_H__ */
