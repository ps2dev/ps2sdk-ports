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

#ifdef SAVE_RCSID
static char rcsid =
 "@(#) $Id$";
#endif

#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "SDL_error.h"
#include "SDL_timer.h"
#include "SDL_timer_c.h"

#include <kernel.h>

/* 
	NOTE:

	Very quick hack to get my game working. more work is needed!
*/

void SDL_StartTicks(void)
{
}

Uint32 SDL_GetTicks(void)
{
	return 0;
}

static void alarm_callback(u32 alarm_id, u16 time, void *data)
{
	int id = (int)data;

	if (id > 0)
	{
		iSignalSema(id);
	}
}

void SDL_Delay(Uint32 ms)
{
	/* this is done using PS2SDK calls directly, since
	 * SDL does not support setting initial count to zero
	 */
	int id;
	ee_sema_t sem;

	sem.init_count = 0;
	sem.max_count = 1;
	sem.option = 0;
	sem.attr = 0;
	id = CreateSema(&sem);
	if (id <= 0)
	{
		/* error :( */
		return;
	}

	SetAlarm(ms * 12, (void *)alarm_callback, (void *)id);
	WaitSema(id);
	DeleteSema(id);
}

/* This is only called if the event thread is not running */
int SDL_SYS_TimerInit(void)
{
	return 0;
}

void SDL_SYS_TimerQuit(void)
{
}

int SDL_SYS_StartTimer(void)
{
	return(-1);
}

void SDL_SYS_StopTimer(void)
{
	return;
}
