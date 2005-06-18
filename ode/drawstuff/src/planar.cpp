/*************************************************************************
 *                                                                       *
 * Open Dynamics Engine, Copyright (C) 2001,2002 Russell L. Smith.       *
 * All rights reserved.  Email: russ@q12.org   Web: www.q12.org          *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of EITHER:                                  *
 *   (1) The GNU Lesser General Public License as published by the Free  *
 *       Software Foundation; either version 2.1 of the License, or (at  *
 *       your option) any later version. The text of the GNU Lesser      *
 *       General Public License is included with this library in the     *
 *       file LICENSE.TXT.                                               *
 *   (2) The BSD-style license that is included with this library in     *
 *       the file LICENSE-BSD.TXT.                                       *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files    *
 * LICENSE.TXT and LICENSE-BSD.TXT for more details.                     *
 *                                                                       *
 *************************************************************************/

#include <ode/config.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <drawstuff/drawstuff.h>
#include <drawstuff/version.h>
#include "internal.h"

#include <llibPlanar.h>

static int width=640,height=480;	// window size
static int run=1;			// 1 if simulation running
static int singlestep=0;		// 1 if single step key pressed
static int pause=0;			// 1 if in `pause' mode

//***************************************************************************
// error handling for unix

static void printMessage (char *msg1, char *msg2, va_list ap)
{
  fflush (stderr);
  fflush (stdout);
  fprintf (stderr,"\n%s: ",msg1);
  vfprintf (stderr,msg2,ap);
  fprintf (stderr,"\n");
  fflush (stderr);
}


extern "C" void dsError (char *msg, ...)
{
  va_list ap;
  va_start (ap,msg);
  printMessage ("Error",msg,ap);
  exit (1);
}


extern "C" void dsDebug (char *msg, ...)
{
  va_list ap;
  va_start (ap,msg);
  printMessage ("INTERNAL ERROR",msg,ap);
  // *((char *)0) = 0;	 ... commit SEGVicide ?
  abort();
}


extern "C" void dsPrint (char *msg, ...)
{
  va_list ap;
  va_start (ap,msg);
  vprintf (msg,ap);
}

void dsPlatformSimLoop (int window_width, int window_height, dsFunctions *fn,
			int initial_pause)
{

  printf ("drawstuff: dsPlatformSimLoop(%d, %d, ..)\n", window_width, window_height);

  gfxInitVideo(width,height);
  gfxSetClearColor(0x00,0x00,0x00);

  dsStartGraphics (window_width,window_height,fn);
  if (fn->start) fn->start();

  while (run) {
    dsDrawFrame (width,height,fn,pause && !singlestep);
    singlestep = 0;

    gfxSyncV();
    gfxSwapBuffers();
    gfxClear();

    // Activamos transparencia
    gfxSetAlphaBlending(1);
    

/* just trying to get output from planar right now */
    gfxSetColor(0xFF, 0x00, 0x00, 0x40);        // RED
    gfxDrawBox(-150,-150, 50, 50);

    gfxSetColor(0x00, 0xFF, 0x00, 0x40);        // GREEN
    gfxDrawBox(-100,-100,100,100);

    gfxSetColor(0x00, 0x00, 0xFF, 0x40);        // BLUE
    gfxDrawBox(- 50,-50,150,150);

    gfxRender();       // enviamos todo el flujo al GS

//    printf (".");
  };

  dsStopGraphics();

}

extern "C" void dsStop()
{
//  run = 0;
}
