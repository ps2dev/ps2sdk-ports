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

    Gil Megidish
    gil@megidish.net

    Sam Lantinga
    slouken@libsdl.org
*/

#ifdef SAVE_RCSID
static char rcsid =
 "@(#) $Id$";
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL_error.h"
#include "SDL_joystick.h"
#include "SDL_sysjoystick.h"
#include "SDL_joystick_c.h"

#include <tamtypes.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>

#include "libpad.h" ////// FIXME

#define MAX_JOYSTICKS	8
#define MAX_AXES	4	/* 2 analog sticks x 2 axes each  */
#define MAX_BUTTONS	12	/* and 12 buttons                  */
#define	MAX_HATS	2

#define	JOYNAMELEN	8

#define AXIS_THRESHOLD  0

/* array to hold joystick ID values */
static char padbufs[MAX_JOYSTICKS*256] __attribute__((aligned(64)));

static int joyports[MAX_JOYSTICKS];
static int joyslots[MAX_JOYSTICKS];

static const int sdl_buttons[MAX_BUTTONS] = 
{
	PAD_SQUARE,
	PAD_CROSS,
	PAD_CIRCLE,
	PAD_TRIANGLE,
	PAD_SELECT,
	PAD_START,
	PAD_L1,
	PAD_R1,
	PAD_L2,
	PAD_R2,
	PAD_L3,
	PAD_R3
};	

struct joystick_hwdata
{
	int port;
	int slot;
	int prev_buttons;
	int prev_ljoy_h;
	int prev_ljoy_v;
	int prev_rjoy_h;
	int prev_rjoy_v;
};
                    
static int wait_pad(int port, int slot)
{
	int tries;
	int state, last_state;
	char state_string[16];

	state = padGetState(port, slot);
	if (state == PAD_STATE_DISCONN)
	{
		printf("SDL_Joystick: pad (%d, %d) is disconnected\n", port, slot);
		return -1;
	}

	last_state = -1;

	tries = 65536;
	while ((state != PAD_STATE_STABLE) && (state != PAD_STATE_FINDCTP1)) 
	{
		if (state != last_state) 
		{
			padStateInt2String(state, state_string);
			printf("SDL_Joystick: pad (%d,%d) is in state %s\n", port, slot, state_string);
		}

		last_state = state;
		state = padGetState(port, slot);

		tries--;
		if (tries == 0)
		{
			printf("waited too long! giving up\n");
			break;
		}
	}

	return 0;
}

/* Function to scan the system for joysticks.
 * This function should set SDL_numjoysticks to the number of available
 * joysticks.  Joystick 0 should be the system default joystick.
 * It should return number of joysticks, or -1 on an unrecoverable fatal error.
 */
int SDL_SYS_JoystickInit(void)
{
	int ret;
	int id;
	int index;
	int numports, numdevs;
	int port, slot;

	printf("SDL_Joystick: JoystickInit begins\n");

	ret = SifLoadModule("rom0:SIO2MAN", 0, NULL);
	if (ret < 0)
	{
		SDL_SetError("Failed to load SIO2MAN");
		return 0;
	}

	ret = SifLoadModule("rom0:PADMAN", 0, NULL);
	if (ret < 0)
	{
		SDL_SetError("Failed to load PADMAN");
		return 0;
	}

	padInit(0);

	numdevs = 0;
	numports = padGetPortMax();
	printf("SDL_Joystick: numports %d\n", numports);
	if (numports > 1)
	{
		/* multitap not supported yet :| */
	}

	index = 0;

	for (port=0; port<numports; port++)
	{
		int maxslots;

		maxslots = padGetSlotMax(port);
		for (slot=0; slot<maxslots; slot++)
		{
			ret = padPortOpen(port, slot, &padbufs[256*index]);
			if (ret < 0)
			{
				//SDL_SetError("padPortOpen %d, %d failed\n", port, slot);
				//return 0;
				continue;
			}

			wait_pad(port, slot);

			id = padInfoMode(port, slot, PAD_MODECURID, 0);
			if (id != 0)
			{
				int ext;

				ext = padInfoMode(port, slot, PAD_MODECUREXID, 0);
				if (ext != 0)
				{
					id = ext;
				}

				if (id == PAD_TYPE_DIGITAL)
				{
					printf("SDL_Joystick: digital pad detected\n");
				}
				else if (id == PAD_TYPE_DUALSHOCK)
				{
					printf("SDL_Joystick: dualshock detected\n");
				}
				else 
				{
					printf("SDL_Joystick: unknown identifier %d detected\n", id);
				}

				/*
				if (0)
				if (id == PAD_TYPE_DIGITAL || id == PAD_TYPE_DUALSHOCK)
				{
					ret = padSetMainMode(port, slot, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK);
					if (ret == 1) 
					{ 
						printf("JoystickInit: Request received\n"); 
					} 
					else
					{ 
						printf("not received!!!\n"); 
					}
				*/

				wait_pad(port, slot);

				printf("Joystick %d at port=%d slot=%d\n", index, port, slot);
				joyports[index] = port;
				joyslots[index] = slot;
				index++;
			}
		}
	}
 	
	SDL_numjoysticks = index;
	printf("SDL_Joystick: JoystickInit ends with %d joysticks\n", SDL_numjoysticks);
	return SDL_numjoysticks;
}

/* Function to get the device-dependent name of a joystick */
const char *SDL_SYS_JoystickName(int index)
{
	return "PS2 Joystick";
}

/* Function to open a joystick for use.
   The joystick to open is specified by the index field of the joystick.
   This should fill the nbuttons and naxes fields of the joystick structure.
   It returns 0, or -1 if there is an error.
 */
int SDL_SYS_JoystickOpen(SDL_Joystick *joystick)
{
	/* allocate memory for system specific hardware data */
	joystick->hwdata = (struct joystick_hwdata *)malloc(sizeof(*joystick->hwdata));
	if (joystick->hwdata == NULL)
	{
		SDL_OutOfMemory();
		return(-1);
	}

	memset(joystick->hwdata, 0, sizeof(*joystick->hwdata));

	/* fill nbuttons, naxes, and nhats fields */
	joystick->nbuttons = MAX_BUTTONS;
	joystick->naxes = MAX_AXES;
	joystick->nhats = MAX_HATS;
	joystick->hwdata->port = joyports[joystick->index];
	joystick->hwdata->slot = joyslots[joystick->index];
	return(0);
}

/* Function to update the state of a joystick - called as a device poll.
 * This function shouldn't update the joystick structure directly,
 * but instead should call SDL_PrivateJoystick*() to deliver events
 * and update joystick device state.
 */

void SDL_SYS_JoystickUpdate(SDL_Joystick *joystick)
{
	int i, ret;
	int port, slot;
	int rjoy_h, rjoy_v;
	int ljoy_h, ljoy_v;
	struct padButtonStatus buttons;
	u32 paddata;
	u32 old_pad;
	u32 new_pad;

	port = joystick->hwdata->port;
	slot = joystick->hwdata->slot;

	wait_pad(port, slot);
        ret = padRead(port, slot, &buttons); 
	if (ret != 0)
	{
		int changed, hat;

		old_pad = joystick->hwdata->prev_buttons;
		paddata = 0xffff ^ buttons.btns;
		new_pad = paddata; 
		changed = paddata ^ old_pad;
		joystick->hwdata->prev_buttons = paddata;

		hat = SDL_HAT_CENTERED;
		if (new_pad & PAD_LEFT)
		{
			hat = hat | SDL_HAT_LEFT;
		}

		if (new_pad & PAD_RIGHT)
		{
			hat = hat | SDL_HAT_RIGHT;
		}

		if (new_pad & PAD_DOWN)
		{
			hat = hat | SDL_HAT_DOWN;
		}

		if (new_pad & PAD_UP)
		{
			hat = hat | SDL_HAT_UP;
		}

		for (i=0; i<MAX_BUTTONS; i++)
		{
			if (changed & sdl_buttons[i])
			{
				int status = (new_pad & sdl_buttons[i]) ? SDL_PRESSED : SDL_RELEASED;

				SDL_PrivateJoystickButton(joystick, i, status);
			}
		}

		if (changed & (PAD_LEFT|PAD_RIGHT|PAD_UP|PAD_DOWN))
		{
			SDL_PrivateJoystickHat(joystick, 0, hat);
		}

		/* now do axis */
		ljoy_h = buttons.ljoy_h - 128;
		ljoy_v = buttons.ljoy_v - 128;
		rjoy_h = buttons.rjoy_h - 128;
		rjoy_v = buttons.rjoy_v - 128;

		/*
		printf("rjoy_h %d rjoy_v %d ljoy_h %d ljoy_v %d (%d, %d, %d, %d)\n",
		rjoy_h, rjoy_v,
		ljoy_h, ljoy_v,
		joystick->hwdata->prev_rjoy_h, joystick->hwdata->prev_rjoy_v,
		joystick->hwdata->prev_ljoy_h, joystick->hwdata->prev_ljoy_v);
		*/

		/* left analog stick */
		if (abs(joystick->hwdata->prev_ljoy_h - ljoy_h) > AXIS_THRESHOLD)
		{
			SDL_PrivateJoystickAxis(joystick, 0, ljoy_h * 127);
			joystick->hwdata->prev_ljoy_h = ljoy_h;
		}

		if (abs(joystick->hwdata->prev_ljoy_v - ljoy_v) > AXIS_THRESHOLD)
		{
			SDL_PrivateJoystickAxis(joystick, 1, ljoy_v * 127);
			joystick->hwdata->prev_ljoy_v = ljoy_v;
		}

		/* right analog stick */
		if (abs(joystick->hwdata->prev_rjoy_h - rjoy_h) > AXIS_THRESHOLD)
		{
			SDL_PrivateJoystickAxis(joystick, 2, rjoy_h  * 127);
			joystick->hwdata->prev_rjoy_h = rjoy_h;
		}

		if (abs(joystick->hwdata->prev_rjoy_v - rjoy_v) > AXIS_THRESHOLD)
		{
			SDL_PrivateJoystickAxis(joystick, 3, rjoy_v * 127);
			joystick->hwdata->prev_rjoy_v = rjoy_v;
		}
	} 
	else
	{
		SDL_SetError("JoystickUpdate failed\n");
	}
}

/* Function to close a joystick after use */
void SDL_SYS_JoystickClose(SDL_Joystick *joystick)
{
}

/* Function to perform any system-specific joystick related cleanup */
void SDL_SYS_JoystickQuit(void)
{
}
