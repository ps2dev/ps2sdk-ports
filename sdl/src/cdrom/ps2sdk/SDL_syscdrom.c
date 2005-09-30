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

/* Functions for system-level CD-ROM audio control */

#include <stdlib.h>
#include <stdio.h>

#include <SDL.h>
#include "SDL_error.h"
#include <SDL_cdrom.h>
#include "SDL_syscdrom.h"

#include <audsrv.h>

static const char *SDL_SYS_CDName(int drive)
{
	return "cdrom:";
}

static int SDL_SYS_CDOpen(int drive)
{
	return(drive);
}

static int SDL_SYS_CDGetTOC(SDL_CD *cdrom)
{
	int i;

	cdrom->numtracks = audsrv_get_numtracks();
	for (i=0; i<cdrom->numtracks; i++)
	{
		cdrom->track[i].id = i;
		cdrom->track[i].type = SDL_AUDIO_TRACK; // FIXME;
		cdrom->track[i].offset = audsrv_get_track_offset(i);
		cdrom->track[i].length = audsrv_get_track_offset(i+1) - cdrom->track[i].offset;
	}

	return 0;
}

static CDstatus SDL_SYS_CDStatus(SDL_CD *cdrom, int *position)
{
	int status = audsrv_get_cd_status();

	switch(status)
	{
		case 0x00:
		/* stopped */
		return CD_STOPPED;

		case 0x01:
		/* tray open */
		return CD_TRAYEMPTY;

		case 0x02:
		case 0x06:
		case 0x12:
		/* spinning, reading, or seeking */
		return CD_PLAYING;

		case 0x0a:
		/* paused */
		return CD_PAUSED;

		case 0x20:
		default:
		/* error or unknown */
		return CD_ERROR;
	}
}

/* Start play */
static int SDL_SYS_CDPlay(SDL_CD *cdrom, int start, int length)
{
	if (audsrv_play_sectors(start, start + length) == 0)
	{
		return 0;
	}

	return -1;
}

/* Pause play */
static int SDL_SYS_CDPause(SDL_CD *cdrom)
{
	audsrv_pause_cd();
	return 0;
}

/* Resume play */
static int SDL_SYS_CDResume(SDL_CD *cdrom)
{
	audsrv_resume_cd();
	return 0;
}

/* Stop play */
static int SDL_SYS_CDStop(SDL_CD *cdrom)
{
	audsrv_stop_cd();
	return 0;
}

/* Eject the CD-ROM */
static int SDL_SYS_CDEject(SDL_CD *cdrom)
{
	return -1;
}

/* Close the CD-ROM handle */
static void SDL_SYS_CDClose(SDL_CD *cdrom)
{
}

void SDL_SYS_CDQuit(void)
{
}

int SDL_SYS_CDInit(void)
{
	printf("SDL CDROM support is a work in progress;\nPlease do not use it at the moment\n");
	SDL_OutOfMemory();
 	return -1;

	/* must have audsrv initialized and ready to go */
	if (SDL_WasInit(SDL_INIT_AUDIO) == 0)
	{
		SDL_SetError("SDL_INIT_AUDIO must be enabled");
		return -1;
	}

	/* Fill in our driver capabilities */
	SDL_numcds = 1;
	SDL_CDcaps.Name = SDL_SYS_CDName;
	SDL_CDcaps.Open = SDL_SYS_CDOpen;
	SDL_CDcaps.GetTOC = SDL_SYS_CDGetTOC;
	SDL_CDcaps.Status = SDL_SYS_CDStatus;
	SDL_CDcaps.Play = SDL_SYS_CDPlay;
	SDL_CDcaps.Pause = SDL_SYS_CDPause;
	SDL_CDcaps.Resume = SDL_SYS_CDResume;
	SDL_CDcaps.Stop = SDL_SYS_CDStop;
	SDL_CDcaps.Eject = SDL_SYS_CDEject;
	SDL_CDcaps.Close = SDL_SYS_CDClose;
	return 0;
}
