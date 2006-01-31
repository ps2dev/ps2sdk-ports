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

#include <audsrv.h>

#ifdef USE_FREESD
extern int freesd_irx_size;
extern char freesd_irx_start[];
#endif

extern int audsrv_irx_size;
extern char audsrv_irx_start[];

static int spu2_init()
{
	int id;
	int error;
	char *iopbuf;
	struct t_SifDmaTransfer sdt;

	SifInitRpc(0);

#ifdef USE_FREESD
	// load freesd (libsd replacement)
    	SifExecModuleBuffer(freesd_irx_start, freesd_irx_size, 0, NULL, &error);
    	if (error < 0)
    	{
		SDL_SetError("Failed to open FREESD module"); 
    	} 
#else
	error = SifLoadModule("rom0:LIBSD", 0, NULL);
	if (error < 0)
	{
		SDL_SetError("Failed to open LIBSD module");
		return -1;
	}
#endif
	iopbuf = (char *)SifAllocIopHeap(audsrv_irx_size);
	if (iopbuf == NULL)
	{
		SDL_SetError("Failed to allocate IOP memory");
		return -1;
	}

	sdt.src = (void *)audsrv_irx_start;
	sdt.dest = (void *)iopbuf;
	sdt.size = audsrv_irx_size;
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
		SDL_SetError("Failed to open audsrv module");
		return -1;
	}

	/* init audsrv */
	audsrv_init();

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
	free(device->hidden);
	free(device);
}

static int fillbuf_callback(void *arg)
{
	SDL_AudioDevice *device = (SDL_AudioDevice *)arg;

	if (device->hidden->sema >= 0)
	{
		iSignalSema(device->hidden->sema);
	}

	return 0;
}	

/* block until can write a full sound buffer */
static void PS2AUD_WaitAudio(_THIS)
{
	if (this->hidden->playing == 0)
	{
		/* not even playing */
		return;
	}

//	WaitSema(this->hidden->sema);
	audsrv_wait_audio(this->hidden->mixlen);
}

static void PS2AUD_PlayAudio(_THIS)
{
	if (this->hidden->playing == 0)
	{
		this->hidden->playing = 1;
	}

	audsrv_play_audio(this->hidden->mixbuf, this->hidden->mixlen);

	#if 0
	/* uncomment for debugging purposes only */
	{
		static FILE *fp = 0;
		if (fp == 0) fp = fopen("foo", "wb");
		fwrite(this->hidden->mixbuf, this->hidden->mixlen, 1, fp);
	}
	#endif
}

static Uint8 *PS2AUD_GetAudioBuf(_THIS)
{
	return(this->hidden->mixbuf);
}

static int CreateMutex(int state)
{
	ee_sema_t sem;

	sem.init_count = state;
	sem.max_count = 1;
	sem.option = 0;
	sem.attr = 0;
	return CreateSema(&sem);
}

static void PS2AUD_CloseAudio(_THIS)
{
}

static int PS2AUD_OpenAudio(_THIS, SDL_AudioSpec *spec)
{
	//Uint16 format;

	audsrv_fmt_t fmt; 
	fmt.freq = spec->freq;
	fmt.bits = spec->format & 0xff;
	fmt.channels = spec->channels;
	if (audsrv_set_format(&fmt) != AUDSRV_ERR_NOERROR)
	{
		SDL_SetError("Unsupported audio format");
		return -1;
	}

	SDL_CalculateAudioSpec(spec);

	/* Allocate mixing buffer */
	this->hidden->mixlen = spec->size;
	this->hidden->mixbuf = (Uint8 *)SDL_AllocAudioMem(this->hidden->mixlen * 4);
	if (this->hidden->mixbuf == NULL)
	{
		free(this->hidden);
		this->hidden = 0;

		SDL_OutOfMemory();
		return(-1);
	}

	this->hidden->sema = CreateMutex(0);
	if (this->hidden->sema < 0)
	{
		free(this->hidden->mixbuf);
		free(this->hidden);
		this->hidden = 0;

		SDL_OutOfMemory();
		return(-1);
	}

	memset(this->hidden->mixbuf, spec->silence, spec->size);

//	audsrv_on_fillbuf(this->hidden->mixlen, fillbuf_callback, (void *)this);

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
