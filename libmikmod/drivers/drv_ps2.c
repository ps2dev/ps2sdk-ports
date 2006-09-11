/*	MikMod sound library
	(c) 1998, 1999, 2000 Miodrag Vallat and others - see file AUTHORS for
	complete list.

	This library is free software; you can redistribute it and/or modify
	it under the terms of the GNU Library General Public License as
	published by the Free Software Foundation; either version 2 of
	the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Library General Public License for more details.

	You should have received a copy of the GNU Library General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
	02111-1307, USA.
*/

/*==============================================================================

  Output data to PS2 audio device
  Freesd & Audsrv Powered !

==============================================================================*/


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mikmod_internals.h"

#include <string.h>
#include <kernel.h>
#include <sifrpc.h>
#include <iopheap.h>
#include <loadfile.h>
#include <tamtypes.h>
#include <audsrv.h>

#ifdef USE_FREESD
extern int freesd_irx_size;
extern char freesd_irx_start[];
#endif

extern int audsrv_irx_size;
extern char audsrv_irx_start[];


#define STACK_SIZE              16384
#define PS2_NUM_AUDIO_SAMPLES   512L

static char mikmod_sndbuf[PS2_NUM_AUDIO_SAMPLES * 4];

extern void *_gp;

static int ee_threadID = 0;
static int ee_mainThreadID;
static ee_thread_t th_attr;
static ee_thread_t currentThread;

// default thread priority 
static int th_attr_priority = 64;

// some status flag
static volatile int playing = 0;




static int spu2_init()
{
	int error;

        #ifdef USE_FREESD
	// load freesd (libsd replacement)
    	SifExecModuleBuffer(freesd_irx_start, freesd_irx_size, 0, NULL, &error);
    	if (error < 0)
    	{
		printf("libmikmod: Failed to open FREESD module");
		return -1;
    	} 
        #else
	error = SifLoadModule("rom0:LIBSD", 0, NULL);
	if (error < 0)
	{
		printf("libmikmod: Failed to open LIBSD module");
		return -1;
	}
        #endif
        SifExecModuleBuffer(audsrv_irx_start, audsrv_irx_size, 0, NULL, &error);

	/* init audsrv */
	audsrv_init();

	return 0;
}


static void PS2_Update(void)
{	
	ULONG done = 0;
	
	while (1)
	{	
	   if (playing)
           {
   	      done = VC_WriteBytes((char *)mikmod_sndbuf,PS2_NUM_AUDIO_SAMPLES * 4);
	   }
	   else
           {
              done = VC_SilenceBytes((char *)mikmod_sndbuf,PS2_NUM_AUDIO_SAMPLES * 4);
           }
           audsrv_wait_audio( done);
	   audsrv_play_audio( (char *)mikmod_sndbuf, done);
           
	}
	
	audsrv_stop_audio();
	WakeupThread ( ee_mainThreadID );

}


static BOOL PS2_IsThere(void)
{
	return 1;
}

static BOOL PS2_Init(void)
{

	if (VC_Init())
  	  return 1;
	
	audsrv_fmt_t fmt; 
	
	if (spu2_init() < 0)
	{
		return 1;
	}
	
	fmt.freq = md_mixfreq;
	fmt.bits = ((md_mode&DMODE_16BITS)?16:8) & 0xff;
	fmt.channels = (md_mode&DMODE_STEREO)?2:1;
		
	if (audsrv_set_format(&fmt) != AUDSRV_ERR_NOERROR)
	{
		printf("libmikmod : Audsrv unsupported audio format");
		return 1;
	}
	
	// set software mode for music & sound fx
	md_mode|=DMODE_SOFT_MUSIC|DMODE_SOFT_SNDFX;
	// reset playing flag
	playing = 0;
	
	// setup the driver thread
	th_attr.func = (void *)PS2_Update;
	th_attr.stack = (void *)malloc(STACK_SIZE);
	if (th_attr.stack == NULL)
	{
		printf("libmikmod: error creating update thread\n");
		return(-1);
	}
        
        ee_mainThreadID  = GetThreadId ();
        ReferThreadStatus ( ee_mainThreadID, &currentThread );
        
        // set a higher priority that the main thread
        th_attr_priority = currentThread.current_priority;
        if (th_attr_priority>0) th_attr_priority--;

	th_attr.stack_size = STACK_SIZE;
	th_attr.gp_reg = (void *)&_gp;
	th_attr.initial_priority = th_attr_priority;
	th_attr.option = 0;

	ee_threadID = CreateThread(&th_attr);
	if (ee_threadID < 0) 
	{
		printf("libmikmod: Not enough resources to create update thread");
		return(-1);
	}

	StartThread(ee_threadID, 0);
	printf("libmikmod : thread started with priority 0x%x\n",th_attr_priority);
	// suspend it for now
	SuspendThread(ee_threadID);
	
	printf("libmikmod: Init done\n");
	
	return 0;

}

static void PS2_Exit(void)
{
        // who kill Dr thread...?
        TerminateThread(ee_threadID);
}


static BOOL PS2_Reset(void)
{
	return 0;
}

static BOOL PS2_PlayStart(void)
{
	VC_PlayStart();
	playing = 1;
	ResumeThread(ee_threadID);
	return 0;
}

static void PS2_PlayStop(void)
{
	playing = 0;
	VC_PlayStop();
        SuspendThread(ee_threadID); 
        audsrv_stop_audio();
}

MIKMODAPI MDRIVER drv_ps2 =
{
	NULL,
	"PS2 Audio",
	"PS2 Output Driver v1.0 - by Evilo and the Froggies !",
	0,255,
	"ps2drv",

	NULL,
	PS2_IsThere,
	VC_SampleLoad,
	VC_SampleUnload,
	VC_SampleSpace,
	VC_SampleLength,
  	PS2_Init,
	PS2_Exit,
	PS2_Reset,
  
	VC_SetNumVoices,
	PS2_PlayStart,
	PS2_PlayStop,
	PS2_Update,
  	NULL,
  
	VC_VoiceSetVolume,
	VC_VoiceGetVolume,
	VC_VoiceSetFrequency,
	VC_VoiceGetFrequency,
	VC_VoiceSetPanning,
	VC_VoiceGetPanning,
	VC_VoicePlay,
	VC_VoiceStop,
	VC_VoiceStopped,
	VC_VoiceGetPosition,
	VC_VoiceRealVolume
};



