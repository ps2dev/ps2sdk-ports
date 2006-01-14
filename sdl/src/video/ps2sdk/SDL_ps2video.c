/*
	SDL - Simple DirectMedia Layer
	Copyright (C) 1997, 1998, 1999, 2000  Sam Lantinga

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Library General Public
	License as published by the Free Software Foundation; either
	version 2 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Library General Public License for more details.

	You should have received a copy of the GNU Library General Public
	License along with this library; if not, write to the Free
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	Gil Megidish
	gil@megidish.net

	Based on code by 

	Sam Lantinga
	slouken@libsdl.org
*/

#include "SDL.h"
#include "SDL_error.h"
#include "SDL_video.h"
#include "SDL_sysvideo.h"
#include "SDL_pixels_c.h"

#include "SDL_ps2video.h"

#include "SDL_ps2kbdevents.h"
#include "SDL_ps2USBevents.h"
#include "SDL_ps2mouseevents.h"

#include <string.h>
#include <gsKit.h>
#include <dmaKit.h>

#ifdef SAVE_RCSID
static char rcsid =
 "@(#) $Id$";
#endif

#define REG_VIDEO_MODE   (* (u8 *)0x1fc7ff52)
#define MODE_PAL                        'E'

static int clut_xlut[256] =
{
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
	0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
	0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
	0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
	0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
	0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
	0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
	0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
	0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
	0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
	0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
	0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
	0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
	0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
	0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
	0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
	0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
	0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7,
	0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
	0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
	0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
	0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
	0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
	0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff
};

static const u64 BLACK_RGBAQ = GS_SETREG_RGBAQ(0x00,0x00,0x00,0x00,0x00);
static const u64 TEXTURE_RGBAQ = GS_SETREG_RGBAQ(0x80,0x80,0x80,0x80,0x00);

static u32 gsClut[256] __attribute__ ((aligned(64)));
static GSGLOBAL *gsGlobal = NULL;
static GSTEXTURE gsTexture;

#ifdef PS2SDL_USE_INPUT_DEVICES
/** Initialize USB devices

    Updates SDL error status upon exit
 */
static void initialize_devices(SDL_VideoDevice *device)
{
	/* initialize keyboard and mouse */
	if (PS2_InitUSB(device) >= 0) 
	{
    		if (PS2_InitKeyboard(device) < 0) 
		{
	   		SDL_SetError("Unable to open keyboard");
	    	}

		if (PS2_InitMouse(device) < 0) 
		{
    			SDL_SetError("Unable to open mouse");
		}
    	}       
	else
	{
		/* no mouse, or keyboard */
    		SDL_SetError("Unable to open USB Driver");	
	}
}

#else

/* input devices are disabled, provide stubs */
void PS2_UpdateMouse(_THIS)
{
}

void PS2_InitOSKeymap(SDL_VideoDevice *device)
{
}
#endif

static void clear_screens()
{
	int i;

	for (i=0; i<2; i++)
	{
		/* clear both frames */
		gsKit_clear(gsGlobal, BLACK_RGBAQ);
		gsKit_sync_flip(gsGlobal);
	}
}

static int PS2_VideoInit(SDL_VideoDevice *device, SDL_PixelFormat *vformat)
{
	int pal;

	vformat->BitsPerPixel = 16;
	vformat->BytesPerPixel = 2;
	vformat->Rmask = 0x0000001f;
	vformat->Gmask = 0x000003e0;
	vformat->Bmask = 0x00007c00;
	vformat->Amask = 0x00008000;

	pal = (REG_VIDEO_MODE == MODE_PAL);
	printf("SDL: initializing gsKit in %s mode\n", pal ? "PAL" : "NTSC");
	gsGlobal = gsKit_init_global(pal ? GS_MODE_PAL : GS_MODE_NTSC);

	if (gsGlobal == NULL)
	{
		SDL_SetError("Failed to initialize gsKit");
		return -1;
	}

	/* initialize the DMAC */
	dmaKit_init(D_CTRL_RELE_ON,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC, D_CTRL_STD_OFF, D_CTRL_RCYC_8);
	dmaKit_chan_init(DMA_CHANNEL_GIF);

	/* disable zbuffer because we're only doing 2d now */
	gsGlobal->ZBuffering = GS_SETTING_OFF;

	gsGlobal->DoubleBuffering = GS_SETTING_ON;
	gsGlobal->PrimAAEnable = 0;
	gsGlobal->PrimAlphaEnable = 0;

	gsGlobal->PSM = GS_PSM_CT16;
	gsGlobal->Test->ZTE = 0;

	gsKit_init_screen(gsGlobal);

	clear_screens();

#ifdef PS2SDL_USE_INPUT_DEVICES
	/* initialize keyboard and mouse */
	initialize_devices(device);
#endif
	return 0;
}

static void PS2_VideoQuit(SDL_VideoDevice *device)
{
}

static int PS2_Available(void)
{
	/* Always available ! */
	return 1;
}

static void PS2_DeleteDevice(SDL_VideoDevice *device)
{
	free(device->hidden);
	free(device);
}

static SDL_Rect rect_256x224 = {0, 0, 256, 224};
static SDL_Rect rect_256x256 = {0, 0, 256, 256};
static SDL_Rect rect_320x200 = {0, 0, 320, 200};
static SDL_Rect rect_320x240 = {0, 0, 320, 240};
static SDL_Rect rect_320x256 = {0, 0, 320, 256};
static SDL_Rect rect_400x256 = {0, 0, 400, 256};
static SDL_Rect rect_512x448 = {0, 0, 512, 448};
static SDL_Rect rect_640x200 = {0, 0, 640, 200};
static SDL_Rect rect_640x400 = {0, 0, 640, 400};
static SDL_Rect rect_640x448 = {0, 0, 640, 448};
static SDL_Rect rect_640x480 = {0, 0, 640, 480};
static SDL_Rect rect_800x600 = {0, 0, 800, 600};
static SDL_Rect rect_1024x768 = {0, 0, 1024, 768};
static SDL_Rect rect_1280x1024 = {0, 0, 1280, 1024};
static SDL_Rect *vesa_modes[] = {
	&rect_1280x1024,
	&rect_1024x768,
	&rect_800x600,
	&rect_640x480,
	&rect_640x448,
	&rect_640x400,
	&rect_640x200,
	&rect_512x448,
	&rect_400x256,
	&rect_320x256,
	&rect_320x240,
	&rect_320x200,
	&rect_256x256,
	&rect_256x224,
	NULL
};
	
static SDL_Rect **PS2_ListModes(SDL_VideoDevice *device, SDL_PixelFormat *format, Uint32 flags)
{
	int bpp;
	
	bpp = format->BitsPerPixel;
	if (bpp != 4 && bpp != 8 && bpp != 16 && bpp != 24 && bpp != 32)
	{
		/* PS2 supports 16, 24 and 32bits (we fake 4 and 8) */
		return NULL;
	}

	/* all depths are supported in any resolution */	
	return vesa_modes;
}

static SDL_Surface *PS2_SetVideoMode(SDL_VideoDevice *device, SDL_Surface *current, int width, int height, int bpp, Uint32 flags)
{
	int psm, size;
	int Rmask, Gmask, Bmask, Amask;
	int visible_w, visible_h;
	float ratio, w_ratio, h_ratio;

	printf("SDL_SetVideoMode %d x %d x %d\n", width, height, bpp);

	if (gsTexture.Mem != NULL)
	{
		printf("SDL: Second SetVideoMode\n");
		free(gsTexture.Mem);
		gsTexture.Mem = 0;
	}

	/* keep gcc happy */
	Rmask = Gmask = Bmask = Amask = 0x00000000;

	switch(bpp)
	{
		case 4:
		psm = GS_PSM_T4;
		break;
		
		case 8:
		psm = GS_PSM_T8;
		break;
		
		case 16:
		psm = GS_PSM_CT16; 
		Rmask = 0x0000001f;
		Gmask = 0x000003e0;
		Bmask = 0x00007c00;
		Amask = 0x00000000;
		break;
		
		case 24:
		psm = GS_PSM_CT24;
		Rmask = 0x000000ff;
		Gmask = 0x0000ff00;
		Bmask = 0x00ff0000;
		Amask = 0x00000000;	
		break;
		
		case 32:
		psm = GS_PSM_CT32;
		Rmask = 0x000000ff;
		Gmask = 0x0000ff00;
		Bmask = 0x00ff0000;
		Amask = 0xff000000;
		break;
		
		default:
		SDL_SetError("Unsupported depth");
		return NULL;
	}

	size = gsKit_texture_size(width, height, psm);
//	if (size < 640*400*2) size = 640*400*2;

	gsTexture.Width = width;
	gsTexture.Height = height;
	gsTexture.PSM = psm;
	gsTexture.Mem = (void *)malloc(size);
	if (gsTexture.Mem == NULL)
	{
		SDL_OutOfMemory();
		return NULL;
	}

	memset((void *)gsTexture.Mem, '\0', size);
	gsTexture.Vram = 0x380000;
	//removed: gsKit_vram_alloc(gsGlobal, size);

	printf("SDL_Video: local texture allocated at 0x%08x\n", (unsigned)gsTexture.Mem);

	if (bpp <= 8)
	{
		gsTexture.Clut = gsClut;
		gsTexture.VramClut = 0x300000;
		memset(gsClut, '\0', sizeof(gsClut));
		//removed: (u32)gsKit_vram_alloc(gsGlobal, 1024);
	}
	else
	{
		/* no cluts needed */
		gsTexture.Clut = 0;
		gsTexture.VramClut = 0;
	}

	/* enable bilinear */
	gsTexture.Filter = GS_FILTER_LINEAR;

	//printf("vmem 0x%x, vclut 0x%x, diff %d\n", gsTexture.Vram, gsTexture.VramClut, gsTexture.VramClut - gsTexture.Vram);
	
	if (! SDL_ReallocFormat(current, bpp, Rmask, Gmask, Bmask, Amask)) 
	{
		// FIXME: free GS memory //
		free(gsTexture.Mem);
		gsTexture.Mem = 0;
		SDL_SetError("Couldn't allocate new pixel format for requested mode");
		return(NULL);
	}

	/* set framebuffer */
	current->flags = SDL_FULLSCREEN | SDL_SWSURFACE;
	current->w = width;
	current->h = height;
	current->pitch = SDL_CalculatePitch(current);
	current->pixels = (unsigned char *)gsTexture.Mem;

	/* full screen, with aspect ratio */
	visible_h = gsGlobal->Height;
	visible_w = gsGlobal->Width;
	w_ratio = visible_w / (float)width;
	h_ratio = visible_h / (float)height;
	ratio = (w_ratio <= h_ratio) ? w_ratio : h_ratio;

	device->hidden->ratio = ratio;
	device->hidden->center_x = (visible_w - (width * ratio)) / 2;
	device->hidden->center_y = (visible_h - (height * ratio)) / 2;

	printf("center_x %d center_y %d\n", device->hidden->center_x, device->hidden->center_y);

	clear_screens();
	SDL_SetCursor(0);
	return current;
}

static int PS2_AllocHWSurface(SDL_VideoDevice *device, SDL_Surface *surface)
{
	printf("allochwsurface not implemented :)\n");
	return -1;
}

static int PS2_LockHWSurface(SDL_VideoDevice *device, SDL_Surface *surface)
{
	printf("lockhwsurface not implemented :)\n");
	return 0;
}

static void PS2_UnlockHWSurface(SDL_VideoDevice *device, SDL_Surface *surface)
{
	/* do nothing */
	printf("unlockhwsurface not implemented :)\n");
}

static void PS2_FreeHWSurface(SDL_VideoDevice *device, SDL_Surface *surface)
{
	/* do nothing */
	printf("freehwsurface not implemented :)\n");
}

static void PS2_PumpEvents(SDL_VideoDevice *device)
{
#ifdef PS2SDL_USE_INPUT_DEVICES
	PS2_PumpKeyboardEvents(device);
	PS2_PumpMouseEvents(device);       
#endif
}

static int PS2_SetColors(SDL_VideoDevice *device, int firstcolor, int ncolors, SDL_Color *colors)
{
	int i;
	int rgb;
	int offset;
	int r, g, b;

	for (i=0; i<ncolors; i++)
	{
		r = colors[i].r;
		g = colors[i].g;
		b = colors[i].b;

		rgb = b << 16 | g << 8 | r << 0;

		/* transform into clut's array */
		offset = clut_xlut[firstcolor + i];
		gsClut[offset] = rgb;
	}

	return 0;
}

static void PS2_UpdateRects(SDL_VideoDevice *device, int numrects, SDL_Rect *rects)
{
	int i;
	float ratio;
	int center_x, center_y;
	
	if (gsGlobal == NULL)
	{
		SDL_SetError("Null gsGlobal");
		return;
	}

	ratio = device->hidden->ratio;
	center_x = device->hidden->center_x;
	center_y = device->hidden->center_y;

	/* send new texture via dma */
	gsKit_texture_upload(gsGlobal, &gsTexture);

	/* render portions of texture in the meanwhile */
	for (i=0; i<numrects; i++)
	{
		int x1, y1, x2, y2;
		int u1, v1, u2, v2;
		
		u1 = rects[i].x;
		v1 = rects[i].y;
		u2 = rects[i].x + rects[i].w;
		v2 = rects[i].y + rects[i].h;

		x1 = (rects[i].x * ratio) + center_x;
		y1 = (rects[i].y * ratio) + center_y;
		x2 = ((rects[i].x + rects[i].w) * ratio) + center_x;
		y2 = ((rects[i].y + rects[i].h) * ratio) + center_y;

		gsKit_prim_sprite_texture(gsGlobal, &gsTexture, x1, y1, u1, v1, x2, y2, u2, v2, 1.0, TEXTURE_RGBAQ);
	}

	gsKit_sync_flip(gsGlobal);
}

static int PS2_FlipHWSurface(SDL_VideoDevice *device, SDL_Surface *surface)
{
	printf("not sure fliphw works :)\n");
	gsKit_sync_flip(gsGlobal);
	return 0;
}

static SDL_VideoDevice *PS2_CreateDevice(int devindex)
{
	SDL_VideoDevice *device;

	/* Initialize all variables that we clean on shutdown */
	device = (SDL_VideoDevice *)malloc(sizeof(SDL_VideoDevice));
	if (device != NULL) 
	{
		memset(device, 0, (sizeof *device));
		device->hidden = (struct SDL_PrivateVideoData *)malloc((sizeof *device->hidden));
	}

	if ((device == NULL) || (device->hidden == NULL)) 
	{
		SDL_OutOfMemory();
		if (device != NULL) 
		{
			free(device);
		}

		return NULL;
	}

	device->VideoInit = PS2_VideoInit;
	device->ListModes = PS2_ListModes;
	device->SetVideoMode = PS2_SetVideoMode;
	device->SetColors = PS2_SetColors;
	device->VideoQuit = PS2_VideoQuit;
	device->AllocHWSurface = PS2_AllocHWSurface;
	device->LockHWSurface = PS2_LockHWSurface;
	device->UnlockHWSurface = PS2_UnlockHWSurface;
	device->FreeHWSurface = PS2_FreeHWSurface;
	device->InitOSKeymap = PS2_InitOSKeymap;
	device->UpdateMouse = PS2_UpdateMouse;	
	device->PumpEvents = PS2_PumpEvents;
	device->UpdateRects = PS2_UpdateRects;
	device->FlipHWSurface = PS2_FlipHWSurface;
	device->free = PS2_DeleteDevice;

	memset(device->hidden, '\0', (sizeof *device->hidden));
	return device;
}

VideoBootStrap PS2SDK_bootstrap = {
	"ps2sdk", 
	"PlayStation 2",
	PS2_Available, 
	PS2_CreateDevice
};
