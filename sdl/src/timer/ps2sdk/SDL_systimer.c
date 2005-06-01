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

#define INTC_TIM1  	10
#define T1_COUNT        (volatile u32 *)0x10000800
#define T1_MODE         (volatile u32 *)0x10000810
#define T1_COMP         (volatile u32 *)0x10000820

/** ticks since starting SDL */
static int ticks = 0;

/** tim1 handler id */
static int tim1_handler_id = -1;

void SDL_StartTicks(void)
{
	ticks = 0;
}

Uint32 SDL_GetTicks(void)
{
	return ticks;
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

static int ms_handler(int ca)
{
	ticks++;

	/* reset counter */
	*T1_COUNT = 0;
	/* reset interrupt */
	*T1_MODE |= (1 << 10);
	__asm__ volatile("sync.l; ei");
	return 0;
}

/* This is only called if the event thread is not running */
int SDL_SYS_TimerInit(void)
{
	printf("initializing timer..\n");
	tim1_handler_id = AddIntcHandler(INTC_TIM1, ms_handler, 0);
	EnableIntc(INTC_TIM1);

	*T1_COUNT = 0;
	*T1_COMP = 586; /* 150MHZ / 256 / 1000 */
	*T1_MODE = 2 | (0<<2) | (0<<6) | (1<<7) | (1<<8);

	printf("timer init ended okay\n");
	return 0;
}

void SDL_SYS_TimerQuit(void)
{
	DisableIntc(INTC_TIM1);

	if (tim1_handler_id >= 0)
	{
		RemoveIntcHandler(INTC_TIM1, tim1_handler_id);
		tim1_handler_id = -1;
	}
}

int SDL_SYS_StartTimer(void)
{
	printf("FIXME: StartTimer not implemented!\n");
	return(-1);
}

void SDL_SYS_StopTimer(void)
{
	printf("FIXME: StopTimer not implemented!\n");
	return;
}
