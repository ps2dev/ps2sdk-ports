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

    based on BERO's code

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

#define MAX_JOYSTICKS	2
#define MAX_AXES	6	/* each joystick can have up to 6 axes */
#define MAX_BUTTONS	12	/* and 8 buttons                      */
#define	MAX_HATS	2

#define	JOYNAMELEN	8

/* array to hold joystick ID values */
static char padbufs[MAX_JOYSTICKS*256] __attribute__((aligned(64)));

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
	int prev_buttons;
};

static int wait_pad(int port, int slot)
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
			printf("Please wait, pad(%d,%d) is in state %s\n", port, slot, state_string);
		}

		last_state = state;
		state = padGetState(port, slot);
	}

	return 0;
}


/* Function to scan the system for joysticks.
 * This function should set SDL_numjoysticks to the number of available
 * joysticks.  Joystick 0 should be the system default joystick.
 * It should return 0, or -1 on an unrecoverable fatal error.
 */
int SDL_SYS_JoystickInit(void)
{
	int ret;
	int numports, numdevs;

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
	if (numports > 1)
	{
		/* multitap not supported yet :| */
	}

	ret = padPortOpen(0, 0, &padbufs[0]);
	if (ret < 0)
	{
		SDL_SetError("padPortOpen %d, %d failed\n", 0, 0);
		return 0;
	}

	wait_pad(0, 0);

	return(1);
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
	joystick->hwdata = (struct joystick_hwdata *) malloc(sizeof(*joystick->hwdata));
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
	struct padButtonStatus buttons;
	u32 paddata;
	u32 old_pad;
	u32 new_pad;

	port = 0;
	slot = 0;

	wait_pad(port, slot);
        ret = padRead(port, slot, &buttons); 
	if (ret != 0)
	{
		int changed, hat;

		old_pad = joystick->hwdata->prev_buttons;
		paddata = 0xffff ^ buttons.btns;
		new_pad = paddata & ~old_pad;
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
