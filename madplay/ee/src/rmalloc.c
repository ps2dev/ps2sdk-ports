/*
    -----------------------------------------------------------------------
    rmalloc.c - PS2MP3. (c) Ryan Kegel, 2004
	-----------------------------------------------------------------------

    This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

*/

#include "malloc.h"
#include "rmalloc.h"

#ifndef NULL
#define NULL	0
#endif

/****************************************************************************
 * rMemory structure.  Just keeps tabs on how much memory is allocated.		*
 ****************************************************************************/
struct rMem
{
	struct
	{
		int rPart;
		void *rLocation;
	} rElement[100];
	unsigned int rUsed;
} rMem;

	
/****************************************************************************
 * Initialize the rmalloc system.											*
 ****************************************************************************/
void rmallocInit()
{
	int i;
	rMem.rUsed = 0;
	for (i=0; i<100; i++)
	{
		rMem.rElement[i].rPart = -1;
		rMem.rElement[i].rLocation = NULL;
	}
}

/****************************************************************************
 * Allocated a size of memory.												*
 ****************************************************************************/
void *rmalloc(int size)
{
	void *user;
	int i;
	user = malloc(size);
	if (user != NULL)
	{
		for (i=0; i<100; i++)
		{
			if (rMem.rElement[i].rPart == -1)
			{
				rMem.rElement[i].rPart = size;
				rMem.rElement[i].rLocation = user;
				break;
			}
		}
		rMem.rUsed += size;
	}
	return user;
}

/****************************************************************************
 * Returns the amount of memory allocated.									*
 ****************************************************************************/
unsigned int rallocated()
{
	return rMem.rUsed;
}

/****************************************************************************
 * Frees a block of memory.													*
 ****************************************************************************/
void rfree(void *user)
{
	int i;
	for (i=0; i<100; i++)
	{
		if (user == rMem.rElement[i].rLocation)
		{
			rMem.rUsed-=rMem.rElement[i].rPart;
			rMem.rElement[i].rPart = -1;
			rMem.rElement[i].rLocation = NULL;
			break;
		}
	}
	free(user);
}



	
