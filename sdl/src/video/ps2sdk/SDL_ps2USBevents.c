/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2004 Sam Lantinga

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

    Sam Lantinga
    slouken@libsdl.org
*/

/*
 	USB events manager
 	Shazz / MJJ
*/

#include "SDL.h"
#include "SDL_ps2USBevents.h"

#include <tamtypes.h>
#include <loadfile.h>

//USB state
#define USB_AVAILABLE 1
#define USB_NOT_AVAILABLE 0
static int usbState = USB_NOT_AVAILABLE;

extern u8 ps2usbd_irx_start[];
extern int ps2usbd_irx_size;

/*
  No homebrew USBD driver available so.. need to load one from somewhere ! :D
  something is needed to set where : host, cdrom0, hdd0/partition/...
  a nice TODO :D
*/

int PS2_InitUSB(_THIS)
{       
	int ret;
    
	printf("[PS2] Init USB driver\n");

	SifExecModuleBuffer(ps2usbd_irx_start, ps2usbd_irx_size, 0, NULL, &ret);
	if (ret < 0) 
	{
		SDL_SetError("[PS2] Failed to load module: usbd_irx\n");
		return -1;
	}  
    
	usbState = USB_AVAILABLE;
    
	printf("[PS2] Init USB driver done\n");
	return 0;
}
