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

    BERO
    bero@geocities.co.jp

    based on generic/SDL_syssem.c

    Sam Lantinga
    slouken@libsdl.org
*/

#ifdef SAVE_RCSID
static char rcsid =
 "@(#) $Id$";
#endif

/* An implementation of semaphores using mutexes and condition variables */

#include <stdlib.h>

#include "SDL_error.h"
#include "SDL_timer.h"
#include "SDL_thread.h"
#include "SDL_systhread_c.h"


#ifdef DISABLE_THREADS

SDL_sem *SDL_CreateSemaphore(Uint32 initial_value)
{
	SDL_SetError("SDL not configured with thread support");
	return (SDL_sem *)0;
}

void SDL_DestroySemaphore(SDL_sem *sem)
{
	return;
}

int SDL_SemTryWait(SDL_sem *sem)
{
	SDL_SetError("SDL not configured with thread support");
	return -1;
}

int SDL_SemWaitTimeout(SDL_sem *sem, Uint32 timeout)
{
	SDL_SetError("SDL not configured with thread support");
	return -1;
}

int SDL_SemWait(SDL_sem *sem)
{
	SDL_SetError("SDL not configured with thread support");
	return -1;
}

Uint32 SDL_SemValue(SDL_sem *sem)
{
	return 0;
}

int SDL_SemPost(SDL_sem *sem)
{
	SDL_SetError("SDL not configured with thread support");
	return -1;
}

#else

#include <stdio.h>
#include <kernel.h>

typedef struct SDL_semaphore
{
	s32 sem;
} SDL_semaphore;

SDL_sem *SDL_CreateSemaphore(Uint32 initial_value)
{       
	s32 ps2id;
	SDL_semaphore *sem;
	ee_sema_t ps2sem;

	sem = (SDL_semaphore *)malloc(sizeof(*sem));
	if (sem == NULL)
	{
		SDL_OutOfMemory();
		return NULL;
	}

	ps2sem.init_count = initial_value;
	ps2sem.max_count = initial_value;
	ps2sem.option = 0;
	ps2sem.attr = 0;

	ps2id = CreateSema(&ps2sem);
	printf("CreateSemaphore returned %d\n", ps2id);
	if (ps2id < 0)
	{
		SDL_SetError("Failed to create PS2EE semaphore");
		free(sem);
		return NULL;
	}

	sem->sem = ps2id;
	return (SDL_sem *)sem;
}

/* WARNING:
   You cannot call this function when another thread is using the semaphore.
*/
void SDL_DestroySemaphore(SDL_sem *sem)
{
	if ( ! sem ) {
		SDL_SetError("Passed a NULL semaphore");
		return;
	}
	
	DeleteSema(sem->sem);
}

int SDL_SemTryWait(SDL_sem *sem)
{
	if ( ! sem ) {
		SDL_SetError("Passed a NULL semaphore");
		return -1;
	}

	if (PollSema(sem->sem) >= 0)
	{
		/* succeeded */
		return 0;
	}
	else
	{
		/* failed */
		return SDL_MUTEX_TIMEDOUT;
	}
}

int SDL_SemWaitTimeout(SDL_sem *sem, Uint32 timeout)
{
	if ( ! sem ) {
		SDL_SetError("Passed a NULL semaphore");
		return -1;
	}

	/* A timeout of 0 is an easy case */
	if ( timeout == 0 ) {
		return SDL_SemTryWait(sem);
	}

	printf("** SDL_SemWaitTimeout not implemented **\n");
	return 0;
}

int SDL_SemWait(SDL_sem *sem)
{
	if ( ! sem ) {
		SDL_SetError("Passed a NULL semaphore");
		return -1;
	}

	WaitSema(sem->sem);
	return 0;
}

Uint32 SDL_SemValue(SDL_sem *sem)
{
	if ( ! sem ) {
		SDL_SetError("Passed a NULL semaphore");
		return -1;
	}

	printf("** SDL_SemValue not implemented **\n");
	return -1;
}

int SDL_SemPost(SDL_sem *sem)
{
	if ( ! sem ) {
		SDL_SetError("Passed a NULL semaphore");
		return -1;
	}

	SignalSema(sem->sem);
	return 0;
}

#endif /* DISABLE_THREADS */
