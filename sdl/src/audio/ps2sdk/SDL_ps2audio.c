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

*/

#ifdef SAVE_RCSID
static char rcsid =
 "@(#) $Id$";
#endif

#include "SDL_audio.h"
#include "SDL_error.h"
#include "SDL_audiomem.h"
#include "SDL_audio_c.h"
#include "SDL_timer.h"
#include "SDL_ps2audio.h"
#include <string.h>
#include <loadfile.h>

#include <kernel.h>
#include <sifrpc.h>

// FIXME
#define	FREESD_IRX                 0xC003245
#define FREESD_INIT                        1
#define FREESD_PLAY                        2
#define FREESD_STOP                        3
#define FREESD_POLL                        4
#define FREESD_SET_VOLUME                  5


static SifRpcClientData_t _freesd_cd;

int freesd_poll()
{
	int res;
	int status;

	res = SifCallRpc(&_freesd_cd, FREESD_POLL, 0, NULL, 0, &status, sizeof(status), NULL, NULL);
	if (res < 0)
	{
		return 0;
	}

	return status;
}

int freesd_init(int *status)
{
	int res;

	res = SifCallRpc(&_freesd_cd, FREESD_INIT, 0, NULL, 0, &status, sizeof(status), NULL, NULL);
	if (res < 0)
	{
		return res;
	}

	return 0;
}

static int spu2_init()
{
	int error, res;
	u32 status;

	SifInitRpc(0);

	error = SifLoadModule("host:FREESD.IRX", 0, NULL);
	if (error < 0)
	{
		SDL_SetError("Failed to open FREESD.IRX module");
		return -1;
	}

	while (((res = SifBindRpc(&_freesd_cd, FREESD_IRX, 0)) >= 0) &&
		(_freesd_cd.server == NULL))
		nopdelay();

	if (res < 0)
	{
		SDL_SetError("Failed to bind RPC");
		return res;
	}

	if (freesd_init(&status) < 0)
	{
		SDL_SetError("Failed to initialize FREESD");
		return -1;
	}

	return 0;
}

/* Audio driver bootstrap functions */
static int PS2AUD_Available(void)
{
	return 1;
}

static void PS2AUD_DeleteDevice(SDL_AudioDevice *device)
{
	free(device->hidden);
	free(device);
}

/* This function waits until it is possible to write a full sound buffer */
static void PS2AUD_WaitAudio(_THIS)
{
	printf("WaitAudio\n");
	if (this->hidden->playing) 
	{
		while (1)
		{
			/* wait */
			SDL_Delay(1);
		}
	}
}

static void PS2AUD_PlayAudio(_THIS)
{
}

static Uint8 *PS2AUD_GetAudioBuf(_THIS)
{
	return(this->hidden->mixbuf);
}

static void PS2AUD_CloseAudio(_THIS)
{
}

static int PS2AUD_OpenAudio(_THIS, SDL_AudioSpec *spec)
{
	switch(spec->format&0xff) 
	{
		case  8: 
		spec->format = AUDIO_S8; 
		break;

		case 16: 
		spec->format = AUDIO_S16LSB; 
		break;

		default:
		SDL_SetError("Unsupported audio format");
		return -1;
	}

	/* Update the fragment size as size in bytes */
	SDL_CalculateAudioSpec(spec);

	/* Allocate mixing buffer */
	this->hidden->mixlen = spec->size;
	this->hidden->mixbuf = (Uint8 *) SDL_AllocAudioMem(this->hidden->mixlen);
	if (this->hidden->mixbuf == NULL)
	{
		return(-1);
	}

	memset(this->hidden->mixbuf, spec->silence, spec->size);

	this->hidden->playing = 0;
	return 0;
}

static SDL_AudioDevice *PS2AUD_CreateDevice(int devindex)
{
	SDL_AudioDevice *this;

	if (spu2_init() < 0)
	{
		return NULL;
	}

	/* Initialize all variables that we clean on shutdown */
	this = (SDL_AudioDevice *)malloc(sizeof(SDL_AudioDevice));
	if (this != NULL)	 
	{
		memset(this, 0, (sizeof *this));
		this->hidden = (struct SDL_PrivateAudioData *)malloc((sizeof *this->hidden));
	}

	if ((this == NULL) || (this->hidden == NULL)) 
	{
		SDL_OutOfMemory();
		if (this != NULL) 
		{
			free(this);
		}

		return 0;
	}

	memset(this->hidden, 0, sizeof(*this->hidden));

	/* Set the function pointers */
	this->OpenAudio = PS2AUD_OpenAudio;
	this->WaitAudio = PS2AUD_WaitAudio;
	this->PlayAudio = PS2AUD_PlayAudio;
	this->GetAudioBuf = PS2AUD_GetAudioBuf;
	this->CloseAudio = PS2AUD_CloseAudio;
	this->free = PS2AUD_DeleteDevice;

	return this;
}

AudioBootStrap PS2AUD_bootstrap = {
	"ps2audio", 
	"Playstation 2 SPU2 audio",
	PS2AUD_Available, 
	PS2AUD_CreateDevice
};
