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

    based on dc/SDL_thread.c by BERO

    Sam Lantinga
    slouken@libsdl.org
*/

#ifdef SAVE_RCSID
static char rcsid =
 "@(#) $Id$";
#endif

/* Thread management routines for SDL */

#include "SDL_error.h"
#include "SDL_thread.h"
#include "SDL_systhread.h"

#ifdef	DISABLE_THREADS
int SDL_SYS_CreateThread(SDL_Thread *thread, void *args)
{
	SDL_SetError("Threads are not supported on this platform");
	return(-1);
}

void SDL_SYS_SetupThread(void)
{
	return;
}

Uint32 SDL_ThreadID(void)
{
	return(0);
}

void SDL_SYS_WaitThread(SDL_Thread *thread)
{
	return;
}

void SDL_SYS_KillThread(SDL_Thread *thread)
{
	return;
}

#else

#include <kernel.h>
#include <stdio.h>
#include <malloc.h>

#define STACK_SIZE 16384
extern void *_gp;

int SDL_SYS_CreateThread(SDL_Thread *thread, void *args)
{
	ee_thread_t th_attr;

	printf("SDL_SYS_CreateThread\n");

	th_attr.func = (void *)SDL_RunThread;
	th_attr.stack = (void *)malloc(STACK_SIZE);
	if (th_attr.stack == NULL)
	{
		SDL_OutOfMemory();
		return(-1);
	}

	th_attr.stack_size = STACK_SIZE;
	th_attr.gp_reg = (void *)&_gp;
	th_attr.initial_priority = 64;
	th_attr.option = 0;

	thread->handle = CreateThread(&th_attr);
	if (thread->handle < 0) 
	{
		SDL_SetError("Not enough resources to create thread");
		return(-1);
	}

	printf("SDL_SYS_CreateThread ends successfully\n");
	StartThread(thread->handle, args);
	printf("Thread started\n");
	return(0);
}

void SDL_SYS_SetupThread(void)
{
	return;
}

Uint32 SDL_ThreadID(void)
{
	return (Uint32)GetThreadId();
}

void SDL_SYS_WaitThread(SDL_Thread *thread)
{
	printf("SDL_SYS_WaitThread called and I don't know what to do!!\n");
}

void SDL_SYS_KillThread(SDL_Thread *thread)
{
	TerminateThread(thread->handle);
}
#endif
