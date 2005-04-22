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
 	PS2 mouse events manager
 	Shazz / MJJ prod
*/

#include <string.h>

#include "SDL.h"
#include "SDL_sysevents.h"
#include "SDL_events_c.h"
#include "SDL_ps2mouseevents.h"

#include <tamtypes.h>
#include <loadfile.h>

#include <libmouse.h>
#include <ps2mouse.h>

#include <libpad.h>

//binary driver
extern u8 ps2mouse_irx_start[];
extern int ps2mouse_irx_size;

//default mouse threshold and acceleration
#define THRES_DEF 1
#define ACC_DEF 1.0

//Mouse states
#define MOUSE_AVAILABLE 1
#define MOUSE_NOT_AVAILABLE 0
#define PAD_AVAILABLE 2

extern int SDL_SYS_JoystickInit(void);

static int mouseState = MOUSE_NOT_AVAILABLE;
static int PADSTEP = 1;

/*
 In case of no mouse available, init a pad !
 */
static int PS2_InitPad()
{
	if (SDL_WasInit(SDL_INIT_JOYSTICK))
	{
		return 0;
	}
	else
	{
		if(SDL_SYS_JoystickInit() > 0)
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}
}

/*
 Load the embedded USB mouse driver and then initialize the mouse
 */
int PS2_InitMouse(_THIS)
{
	int ret;

	printf("[PS2] Init USB Mouse\n");

	// the mouse driver is embedded in the library....
	SifExecModuleBuffer(ps2mouse_irx_start, ps2mouse_irx_size, 0, NULL, &ret);
	if (ret < 0)
	{
		SDL_SetError("[PS2] Failed to load module: PS2MOUSE.IRX\n");

		if(PS2_InitPad() == 0)
		{
			mouseState = PAD_AVAILABLE;
			printf("[PS2] Init PAD Mouse emulation done\n");
			return 0;
		}

		mouseState = MOUSE_NOT_AVAILABLE;
		printf("[PS2] No mouse or pad available\n");
		return -1;
	}
	else
	{
		// Initialize the mouse
		if(PS2MouseInit() < 0) 
		{
			SDL_SetError("[PS2] PS2MouseInit failed\n");

			if (PS2_InitPad() > 0)
			{
				mouseState = PAD_AVAILABLE;
				printf("[PS2] Init PAD Mouse emulation done\n");
			}
			else
			{
				printf("[PS2] Init USB Mouse and Pad emulation failed\n");
				mouseState = MOUSE_NOT_AVAILABLE;
				return -1;
			}
		}
		else
		{
			mouseState = MOUSE_AVAILABLE;
			printf("[PS2] Init USB Mouse done\n");
		}
	}

	return 0;
}

/*
 Callback triggered when the video mode is set (or changed)
 It will recalibrate the mouse boundaries according to video mode width and height
 */
void PS2_UpdateMouse(_THIS)
{
	if (mouseState == MOUSE_AVAILABLE)
	{
		//recalibrate PS2 mouse depending on video resolution, must be called at resolution change...
		int width = this->screen->w;
		int height = this->screen->h;

		// Set mouse boundaries
		PS2MouseSetBoundary(0, width, 0 , height);
		PS2MouseSetReadMode(PS2MOUSE_READMODE_DIFF);

		// Set initial mouse position, threshold and acceleration
		PS2MouseSetPosition(width/2, height/2);
		PS2MouseSetAccel(ACC_DEF);
		PS2MouseSetThres(THRES_DEF);

		printf("[PS2] Mouse calibrated for (0,0,%d,%d)\n[P22] with threshold %d and acceleration %f\n", width, height, THRES_DEF, ACC_DEF);
	}
	else if (mouseState == PAD_AVAILABLE)
	{
		//recalibrate PS2 pad depending on video resolution, must be called at resolution change...
		int width = this->screen->w;
		int height = this->screen->h;

		PADSTEP = width >> 7;
		printf("[PS2] Mouse Pad calibrated for (0,0,%d,%d)\n[P22] with threshold %d and acceleration %f\n", width, height, PADSTEP, 1.0);
	}
}

/*
 Mouse event callback, send mouse states to the SDL mouse management
 */
void PS2_MouseCallback(int button, int dx, int dy){

	int button_state;
	int state_changed;
	int i;
	Uint8 state;

        // Send first mouse coords
	if ( dx || dy ) {
		SDL_PrivateMouseMotion(0, 1, dx, dy);
	}

	// Then see what changed for the buttons
	button_state = SDL_GetMouseState(NULL, NULL);
	state_changed = button_state ^ button;

	for (i=0; i<8; i++) 
	{
		int mask = 1 << i;

		if (state_changed & mask) 
		{
			if (button & mask) 
			{
				state = SDL_PRESSED;
			} 
			else 
			{
				state = SDL_RELEASED;
			}

			// Send button state for each button
			SDL_PrivateMouseButton(state, i+1, 0, 0);
		}
	}
}

/*
 Internal pad fn : manage pad state
 */
static int PS2_WaitPad(int port, int slot)
{
	int state, last_state;
	char state_string[16];

	state = padGetState(port, slot);
	last_state = -1;

	while ((state != PAD_STATE_STABLE) && (state != PAD_STATE_FINDCTP1))
	{
		if (state != last_state)
		{
			padStateInt2String(state, state_string);
			printf("[PS2] Pad(%d,%d) is in state %s\n", port, slot, state_string);
		}

		last_state = state;
		state = padGetState(port, slot);
	}

	return 0;
}

/*
 Read pad informations to simulate mosue movements
 */
void PS2_MousePadRead(int *padbutton, int *x, int *y)
{
	int port, slot;
	struct padButtonStatus buttons;
	u32 paddata;

	port = 0;
	slot = 0;
	*padbutton = 0;

	PS2_WaitPad(port, slot);
	if (padRead(port, slot, &buttons) != 0)
	{
		paddata = 0xffff ^ buttons.btns;

		if (paddata & PAD_LEFT)	
		{
			*x -= PADSTEP;
		}

		if (paddata & PAD_RIGHT)
		{
			*x += PADSTEP;
		}

		if (paddata & PAD_DOWN)	
		{
			*y += PADSTEP;
		}

		if (paddata & PAD_UP)
		{
			*y -= PADSTEP;
		}

		if (paddata & PAD_CROSS)		
		{
			*padbutton = 1;
		}
	}
	else
	{
		SDL_SetError("[PS2] PS2_MousePadRead failed\n");
	}
}

/*
 Delegate event pumper for the mouse, read current mouse data
 */
void PS2_PumpMouseEvents(_THIS)
{
	if (mouseState == MOUSE_AVAILABLE)
	{
        	PS2MouseData mousedata;
         
		// Read current mouse info
		if (PS2MouseRead(&mousedata) != 0)
		{
			PS2_MouseCallback(mousedata.buttons, mousedata.x, mousedata.y);
		}
	}
	else if (mouseState == PAD_AVAILABLE)
	{
		// Read pad info
		int padbutton = 0;
		int x = 0;
		int y = 0;
        
		PS2_MousePadRead(&padbutton, &x, &y);
		PS2_MouseCallback(padbutton, x, y);
	}
}




