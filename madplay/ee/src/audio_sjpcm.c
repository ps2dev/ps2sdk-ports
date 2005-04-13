/*
 * madplay - MPEG audio decoder and player
 * Copyright (C) 2000-2004 Robert Leslie
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id$
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "global.h"

# include <unistd.h>
# include <errno.h>
# include <sjpcm.h>
# include <string.h>
# include <mad.h>
# include <kernel.h>

# include "gettext.h"

# include "audio.h"

# if defined(WORDS_BIGENDIAN)
#  define audio_pcm_s16  audio_pcm_s16be
# else
#  define audio_pcm_s16  audio_pcm_s16le
# endif

#include <sifrpc.h>
#include <sifcmd.h>
#include <loadfile.h>

typedef void (*functionPointer)();

extern void *_gp;

static char const *host;

//int frame_num __attribute__((aligned (16)));

int current_buffer __attribute__((aligned (16))); // total number of ticks played
int buffered __attribute__((aligned (16))); // total number of ticks buffered

ee_thread_t thread __attribute__((aligned (16)));
static char userThreadStack[16*1024] __attribute__((aligned(16)));

volatile int mainPid __attribute__((aligned(16))) = 0; // pid of this thread
volatile int outputPid __attribute__((aligned(16))) = 0; // pid of output thread
int handlerId __attribute__((aligned (16))); // vsync interrupt handler

extern u8 *isjpcm_irx;
extern int size_isjpcm_irx;

int loadModules()
{
	int ret = 0;

	SifExecModuleBuffer(&isjpcm_irx, size_isjpcm_irx, 0, NULL, &ret);
	
	return ret;
}

// taken from gslib
void EnableVSyncCallbacks(void)
{
	asm __volatile__ ("
		di

		addiu $4, $0, 2	 
		addiu $3, $0, 20

		syscall
		nop

		ei
	");
}

// taken from gslib
void RemoveVSyncCallback(unsigned int RemoveID)
{
	asm __volatile__ ("
		di

		addu  $5, $0, %0	# RemoveID will have already been passed into this func in $4
		addiu $4, $0, 2		# 2 = vsync_start
		addiu $3, $0, 17	# RemoveIntcHandler
		syscall
		nop

		ei"
		: 
		: "g" (RemoveID)
		);
}

// taken from gslib
unsigned int AddVSyncCallback(void (*func_ptr)())
{
	unsigned int AddCallbackID;

	asm __volatile__ ("
		di

		# 'func_ptr' param will have been passed in $4, so move it to $5 (needed for syscall)

		addu  $5, $0, %1
		addiu $4, $0, 2		# 2 = vsync_start	

		addiu $6, $0, 0
		addiu $3, $0, 16	# AddIntcHandler

		syscall				# Returns assigned ID in $2
		nop

		addu  %0, $0, $2
		
		addiu $4, $0, 2	 
		addiu $3, $0, 20

		syscall
		nop

		ei"
	: "=r" (AddCallbackID)
	: "g" (func_ptr)
	);

//		la $4, AddCallbackID	# Store ID in var
//		sw $2, 0($4)


	// Enable VSync callbacks if not already enabled
	EnableVSyncCallbacks(); // perhaps this should only be done once, and not here

	return AddCallbackID;
}

#define FRAME_SIZE	48000 // 1 frame
#define FRAMES	50 // 60 for ntsc
#define TICK	(FRAME_SIZE / FRAMES) 	

// store 2 channels 
unsigned short hold[2][FRAME_SIZE] __attribute__((aligned (16)));
unsigned int held __attribute__((aligned (16))); // number of samples buffd 
// ... for incomplete tick

static void output(void *dat)
{
	int offset;

	while (1) 
	{

		SuspendThread(mainPid); // suspend instead of blocking on semaphors.

		offset = (current_buffer % FRAMES) * TICK;

		if ((buffered - current_buffer) > 0) { 
			SjPCM_Enqueue(&(hold[0][offset]), &(hold[1][offset]), TICK, 0); 
			current_buffer++;
		}

		ResumeThread(mainPid);
		SleepThread();

	}
}

int vsync_func(void)
{

	if (outputPid) 
		iWakeupThread(outputPid);

	asm __volatile__ ("ei");

	return 0;
}

static
int init(struct audio_init *init)
{
	int status;

	host = init->path;
	if (host && *host == 0)
		host = 0;

printf ("loadModules \n");
	/* load modules... maybe shouldnt do this here */
	SifInitRpc(0);
	if (loadModules() < 0) {
		printf ("Failed to load modules\n");
		return -1;
	}
printf ("loadModules complete\n");

	/* sound */
	if(SjPCM_Init(1) < 0)
	{ 
		printf("SjPCM Bind failed!!");
		return -1;
	} 

 	SjPCM_Clearbuff();
	SjPCM_Setvol(0x3fff);

	/* buffer */
	bzero(hold, sizeof(hold));
	held = 0;
	buffered = 0;
	current_buffer = 0;

	/* vsync interrupt handler */
	handlerId = AddVSyncCallback((functionPointer)&vsync_func);

	/* thread to output func */
	thread.func = (void *)output;
	thread.stack = userThreadStack;
	thread.stack_size = sizeof(userThreadStack);
	thread.gp_reg = &_gp;
	thread.initial_priority = 48;

	mainPid = GetThreadId();
	ChangeThreadPriority(mainPid, 51);

	printf("about to spawn thread\n");

	outputPid = CreateThread(&thread);
	status = StartThread(outputPid, NULL);

	return 0;

}

static
int config(struct audio_config *config)
{

	config->channels  = 2;
	config->speed     = 48000;
	config->precision = 16;

	return 0;

}

static struct audio_dither left_dither, right_dither;

unsigned int audio_pcm_sjpcm(unsigned short *leftout, unsigned short *rightout, unsigned int nsamples,
			  mad_fixed_t const *left, mad_fixed_t const *right,
			  enum audio_mode mode, struct audio_stats *stats)
{
	unsigned int len;

	len = nsamples;

	if (right) 
	{	/* stereo */

		switch (mode) 
		{

		case AUDIO_MODE_ROUND:

			while (len--) 
			{
				*leftout = audio_linear_round(16, *left++, stats);
				*rightout = audio_linear_round(16, *right++, stats);

				leftout++;
				rightout++;
			}
			break;

		case AUDIO_MODE_DITHER:

			while (len--) 
			{
				*leftout = audio_linear_dither(16, *left++, &left_dither, stats);
				*rightout = audio_linear_dither(16, *right++, &right_dither, stats);

				leftout++;
				rightout++;
			}
			break;

		default:
			return 0;
		}

		return nsamples;
	}
	else {	/* mono */
		switch (mode) {
		case AUDIO_MODE_ROUND:
			while (len--)
				*leftout++ = audio_linear_round(16, *left++, stats);
			break;

		case AUDIO_MODE_DITHER:
			while (len--)
				*leftout++ = audio_linear_dither(16, *left++, &left_dither, stats);
			break;

		default:
			return 0;
		}

		return nsamples;
	}
}

static
int buffer(unsigned short const *leftptr, unsigned short const *rightptr, signed int len)
{

	unsigned int grab;

	while ((buffered - current_buffer) >= (FRAMES - 5))  // leave 5 ticks empty to avoid overrun
	{

		SuspendThread(mainPid);

	}

	while (len > 0) 
	{

		// grab a ticks worth of data
		grab = TICK;

		// or grab enough to complete a tick
		grab = (grab - held) < grab ? (grab - held) : grab; 

		// but dont grab too much
		grab = len < grab ? len : grab; 

		len -= grab;

		// copy the data to the buffer
	    	memcpy(&hold[0][(buffered % FRAMES) * TICK + held], leftptr, grab * 2);
	    	memcpy(&hold[1][(buffered % FRAMES) * TICK + held], rightptr, grab * 2);

		if ((grab + held) == TICK) {

			// a complete tick has been buffered
			buffered++;
			held = 0;
    			leftptr += grab;
    			rightptr += grab;

		} else {

			// only part of a tick stored... remember for next iteration
			held += grab; 

		}

	}

	return 0;
}


int is_playing __attribute__((aligned(16))) = 0;

static
int play(struct audio_play *play)
{
	unsigned short left[MAX_NSAMPLES];
	unsigned short right[MAX_NSAMPLES];
	signed int len;
	int status;

	if (is_playing == 0) {
		is_playing = 1;
		SjPCM_Play();
	}

	len = audio_pcm_sjpcm(left, right, play->nsamples,
			play->samples[0], play->samples[1],
			play->mode, play->stats);

	status = buffer(left, right, len);

	return status;
}

static
int stop(struct audio_stop *stop)
{
	SjPCM_Pause();
	is_playing = 0;

	return 0;
}

void DisableVSyncCallbacks(void)
{
	asm __volatile__ ("
		di

		addiu $4, $0, 2	 
		addiu $3, $0, 21

		syscall
		nop


		ei
	");
}

static
int finish(struct audio_finish *finish)
{
	while (buffered > current_buffer)  // let buffer finish first
		SuspendThread(mainPid);

	DeleteThread(outputPid);
	RemoveVSyncCallback(handlerId);

	SjPCM_Pause();

	DisableVSyncCallbacks(); // perhaps this shouldn't be done

	return 0;
}

int audio_sjpcm(union audio_control *control)
{
	audio_error = 0;

	switch (control->command) 
	{
		case AUDIO_COMMAND_INIT:
			return init(&control->init);

		case AUDIO_COMMAND_CONFIG:
			return config(&control->config);

		case AUDIO_COMMAND_PLAY:
			return play(&control->play);

		case AUDIO_COMMAND_STOP:
			return stop(&control->stop);

		case AUDIO_COMMAND_FINISH:
			return finish(&control->finish);
	}

	return 0;
}
