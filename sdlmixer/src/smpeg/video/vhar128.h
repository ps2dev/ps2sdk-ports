/* ATI Rage128 video hardware acceleration */
#ifndef _VHAR128_H_
#define _VHAR128_H_

#include "SDL.h"

#ifdef __cplusplus
extern "C" {
#endif

struct vhar128_image;

unsigned int vhar128_new();
int vhar128_init(unsigned int handle, unsigned long width, unsigned long height, struct vhar128_image *ring[], int ring_size);
int vhar128_newdecode(unsigned int handle, int back, int forw, int current);
int vhar128_macroblock(unsigned int handle, int mb_x, int mb_y, int intra, int back, int forw, int mv_back_x, int mv_back_y, int mv_forw_x, int mv_forw_y, long runlevel[6][130]);

int vhar128_flush(unsigned int handle);
void vhar128_close(unsigned int handle);
void vhar128_delete();

struct vhar128_image * vhar128_newimage(unsigned int handle, unsigned long width, unsigned long height);
void vhar128_lockimage(unsigned int handle, struct vhar128_image * image, SDL_Overlay * surface);
void vhar128_unlockimage(unsigned int handle, struct vhar128_image * image, SDL_Overlay * surface);
void vhar128_destroyimage(unsigned int handle, struct vhar128_image * image);

#ifdef __cplusplus
};
#endif
#endif /* _VHAR128_H_ */
