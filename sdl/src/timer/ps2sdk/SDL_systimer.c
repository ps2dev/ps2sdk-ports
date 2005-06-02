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

/* since more than one thread is permitted to sleep on
 * SDL_Delay, and Playstation 2 offers no option to re-schedule,
 * we will be putting our threads to sleep and the handler will 
 * wake them up
 */
#define MAX_SLEEPING_THREADS 4
static int sleeping_threads[MAX_SLEEPING_THREADS];

void SDL_StartTicks(void)
{
	ticks = 0;
}

Uint32 SDL_GetTicks(void)
{
	return ticks;
}

void SDL_Delay(Uint32 ms)
{
	int i;

	for (i=0; i<MAX_SLEEPING_THREADS; i++)
	{
		if (sleeping_threads[i] == -1)
		{
			break;
		}
	}

	if (i >= MAX_SLEEPING_THREADS)
	{
		fprintf(stderr, "too many threads are sleeping at SDL_Delay\n");
		return;
	}

	sleeping_threads[i] = GetThreadId();
	while (ms > 0)
	{
		SleepThread();
		ms--;
	}

	sleeping_threads[i] = -1;
}

static int ms_handler(int ca)
{
	int i;

	ticks++;

	for (i=0; i<MAX_SLEEPING_THREADS; i++)
	{
		if (sleeping_threads[i] != -1)
		{
			iWakeupThread(sleeping_threads[i]);
		}
	}

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
	int i;

	printf("initializing timer..\n");

	for (i=0; i<MAX_SLEEPING_THREADS; i++)
	{
		sleeping_threads[i] = -1;
	}

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
