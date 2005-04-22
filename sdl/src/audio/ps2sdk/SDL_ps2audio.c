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

#include <kernel.h>
#include <sifrpc.h>
#include <iopheap.h>
#include <loadfile.h>

#include "sjpcm.h"

extern int sjpcm_irx_size;
extern char sjpcm_irx_start[];

static int tick;

#define REG_VIDEO_MODE   (* (u8 *)0x1fc80000)
#define MODE_PAL                        0xf3

static int spu2_init()
{
	int id;
	int error;
	char *iopbuf;
 	struct t_SifDmaTransfer sdt;

	SifInitRpc(0);

//	printf("REG: 0x%x\n", REG_VIDEO_MODE);
//	if (REG_VIDEO_MODE == MODE_PAL)
	if (1)
	{
		/* pal system, 50 vsyncs */
		tick = 48000 / 50;
	}
	else
	{
		/* ntsc system, 60 vsyncs */
		tick = 48000 / 60;
	}

	error = SifLoadModule("rom0:LIBSD", 0, NULL);
	if (error < 0)
	{
		SDL_SetError("Failed to open LIBSD module");
		return -1;
	}

	iopbuf = (char *)SifAllocIopHeap(sjpcm_irx_size);
	if (iopbuf == NULL)
	{
		SDL_SetError("Failed to allocate IOP memory");
		return -1;
	}

	sdt.src = (void *)sjpcm_irx_start;
	sdt.dest = (void *)iopbuf;
	sdt.size = sjpcm_irx_size;
	sdt.attr = 0;

	/* start DMA transfer */
	FlushCache(0);
	id = SifSetDma(&sdt, 1);
	while (SifDmaStat(id) >= 0)
	{
		/* wait until completion */
		nopdelay();
	}

	error = SifLoadModuleBuffer(iopbuf, 0, NULL);
	if (error < 0)
	{
		SDL_SetError("Failed to open SjPCM module");
		return -1;
	}

	/* init sjpcm in blocking mode */
	SjPCM_Init(0);
 	SjPCM_Clearbuff();
	SjPCM_Setvol(0x0);

	return 0;
}

static int PS2AUD_Available(void)
{
	/* this device is always available */
	return 1;
}

static void PS2AUD_DeleteDevice(SDL_AudioDevice *device)
{
	/* free device structure */
	DeleteSema(device->hidden->waitaudio_sema);
	device->hidden->waitaudio_sema = 0;
	free(device->hidden);
	free(device);
}

/* block until can write a full sound buffer */
static void PS2AUD_WaitAudio(_THIS)
{
	s16 left[960], right[960];
	s16 *lptr, *rptr, *ptr;

	if (this->hidden->playing == 0)
	{
		/* not even playing */
		return;
	}

	if (this->hidden->waitaudio_sema <= 0)
	{
		/* eek, error */
		return;
	} 

	/* wait for a signal from the vsync handler */
	while (this->hidden->mixpos < this->hidden->mixlen)
	{
		if (SjPCM_Available() >= tick)
		{
			int p;

			/* demux left/right */
			ptr = (s16 *)(this->hidden->mixbuf + this->hidden->mixpos);
			lptr = left;
			rptr = right;
	
			for (p=0; p<tick; p++)
			{
				*lptr++ = *ptr++;
				*rptr++ = *ptr++;
			}

			this->hidden->mixpos += tick*2*2;

			/* enqueue packet for sjpcm */
			SjPCM_Enqueue(left, right, tick, 1);
		}
		else
		{
			/* wait for a buffer */
			SDL_Delay(1);
		}
	}

	/* no more buffer to enqueue */
	this->hidden->mixpos = 0;
}

static void PS2AUD_PlayAudio(_THIS)
{
	if (this->hidden->playing == 0)
	{
		this->hidden->playing = 1;
		SjPCM_Setvol(0x3fff);
		SjPCM_Play();
	}
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
	Uint16 format;
	ee_sema_t ps2sem;

	if ((spec->format & 0xff) != 16 || spec->freq != 48000 || spec->channels != 2)
	{
		SDL_SetError("Unsupported audio format");
		return -1;
	}

	/*
	switch(spec->format & 0xff)
	{
		case 8:
		format = AUDIO_S8;
		break;

		case 16:
		format = AUDIO_S16;
		break;

		default:
		SDL_SetError("Unsupported audio format");
		return -1;
	}

	SDL_BuildAudioCVT(&this->convert, format, spec->channels, spec->freq,
		AUDIO_S16LSB, 2, 48000);

	this.convert.needed = 1;
	*/

	/* Update the fragment size as size in bytes */
	SDL_CalculateAudioSpec(spec);
	spec->size = 960*4;

	/* Allocate mixing buffer */
	this->hidden->mixlen = spec->size;
	this->hidden->mixbuf = (Uint8 *)SDL_AllocAudioMem(this->hidden->mixlen);
	if (this->hidden->mixbuf == NULL)
	{
		return(-1);
	}

	memset(this->hidden->mixbuf, spec->silence, spec->size);

	/* initialize a locked semaphore */
	ps2sem.attr = 0;
	ps2sem.init_count = 0;
	ps2sem.max_count = 1;
	ps2sem.option = 0;
	this->hidden->waitaudio_sema = CreateSema(&ps2sem);

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
