/* 

    TestFonts - test dynamic font loading code

    Copyright (C) A. Schiffler, August 2001

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#ifdef WIN32
 #include <windows.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "SDL.h"

#include "SDL_gfxPrimitives.h"

void HandleEvent()
{
	SDL_Event event; 

	/* Check for events */
        while ( SDL_PollEvent(&event) ) {
                        switch (event.type) {
			 case SDL_KEYDOWN:
			 case SDL_QUIT:
                                        exit(0);
                                        break;
			}
	}
}

void ClearScreen(SDL_Surface *screen)
{
	int i;
	/* Set the screen to black */
	if ( SDL_LockSurface(screen) == 0 ) {
		Uint32 black;
		Uint8 *pixels;
		black = SDL_MapRGB(screen->format, 0, 0, 0);
		pixels = (Uint8 *)screen->pixels;
		for ( i=0; i<screen->h; ++i ) {
			memset(pixels, black,
				screen->w*screen->format->BytesPerPixel);
			pixels += screen->pitch;
		}
		SDL_UnlockSurface(screen);
	}
}

#ifdef PSP
#define RELPATH ""
#else
#define RELPATH "../../"
#endif

void Draw(SDL_Surface *screen)
{
 FILE *file;
 char *myfont;
 char mytext[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
 
 /* Black screen */
 ClearScreen(screen);

 /* Load a font and draw with it */
 myfont=(char *)malloc(1792);
 file = fopen(RELPATH "Fonts/5x7.fnt","r");
 fread(myfont,1792,1,file);
 fclose(file);
 gfxPrimitivesSetFont(myfont,5,7);
 stringRGBA(screen,10,10,mytext,255,255,255,255);
 free(myfont);

 /* Load a font and draw with it */
 myfont=(char *)malloc(3328);
 //
 file = fopen(RELPATH "Fonts/7x13.fnt","r");
 fread(myfont,3328,1,file);
 fclose(file);
 gfxPrimitivesSetFont(myfont,7,13);
 stringRGBA(screen,10,30,mytext,255,255,255,255);
 //
 file = fopen(RELPATH "Fonts/7x13B.fnt","r");
 fread(myfont,3328,1,file);
 fclose(file);
 gfxPrimitivesSetFont(myfont,7,13);
 stringRGBA(screen,10,50,mytext,255,255,255,255);
 //
 file = fopen(RELPATH "Fonts/7x13O.fnt","r");
 fread(myfont,3328,1,file);
 fclose(file);
 gfxPrimitivesSetFont(myfont,7,13);
 stringRGBA(screen,10,70,mytext,255,255,255,255);
 //
 free(myfont);

 /* Load a font and draw with it */
 myfont=(char *)malloc(9216);
 //
 file = fopen(RELPATH "Fonts/9x18.fnt","r");
 fread(myfont,9216,1,file);
 fclose(file);
 gfxPrimitivesSetFont(myfont,9,18);
 stringRGBA(screen,10,90,mytext,255,255,255,255);
 //
 file = fopen(RELPATH "Fonts/9x18B.fnt","r");
 fread(myfont,9216,1,file);
 fclose(file);
 gfxPrimitivesSetFont(myfont,9,18);
 stringRGBA(screen,10,110,mytext,255,255,255,255);
 //
 free(myfont);
 
 /* Display by flipping screens */
 SDL_UpdateRect(screen,0,0,0,0);
 
 while (1) {
  
  /* Events */
  HandleEvent();
  

  /* Delay to limit rate */                   
  SDL_Delay(1000);  
 }
}

#ifdef WIN32
 extern char ** __argv;
 extern int __argc;
 int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
#else // non WIN32
 int main ( int argc, char *argv[] )
#endif
{
	SDL_Surface *screen;
	int w, h;
	int desired_bpp;
	Uint32 video_flags;
#ifdef WIN32
	int argc;
	char **argv;

	argv = __argv;
	argc = __argc;
#endif
	/* Title */
	fprintf (stderr,"Font Test\n");

	/* Set default options and check command-line */
#ifndef PSP
	w = 640;
	h = 480;
	video_flags = SDL_FULLSCREEN;
#else
	w = 480;
	h = 272;
	video_flags = SDL_HWSURFACE | SDL_FULLSCREEN;
#endif
	desired_bpp = 0;
	while ( argc > 1 ) {
		if ( strcmp(argv[1], "-width") == 0 ) {
			if ( argv[2] && ((w = atoi(argv[2])) > 0) ) {
				argv += 2;
				argc -= 2;
			} else {
				fprintf(stderr,
				"The -width option requires an argument\n");
				exit(1);
			}
		} else
		if ( strcmp(argv[1], "-height") == 0 ) {
			if ( argv[2] && ((h = atoi(argv[2])) > 0) ) {
				argv += 2;
				argc -= 2;
			} else {
				fprintf(stderr,
				"The -height option requires an argument\n");
				exit(1);
			}
		} else
		if ( strcmp(argv[1], "-bpp") == 0 ) {
			if ( argv[2] ) {
				desired_bpp = atoi(argv[2]);
				argv += 2;
				argc -= 2;
			} else {
				fprintf(stderr,
				"The -bpp option requires an argument\n");
				exit(1);
			}
		} else
		if ( strcmp(argv[1], "-warp") == 0 ) {
			video_flags |= SDL_HWPALETTE;
			argv += 1;
			argc -= 1;
		} else
		if ( strcmp(argv[1], "-hw") == 0 ) {
			video_flags |= SDL_HWSURFACE;
			argv += 1;
			argc -= 1;
		} else
		if ( strcmp(argv[1], "-fullscreen") == 0 ) {
			video_flags |= SDL_FULLSCREEN;
			argv += 1;
			argc -= 1;
		} else
			break;
	}

	/* Force double buffering */
#ifndef PSP
	video_flags |= SDL_DOUBLEBUF;
#endif

	/* Initialize SDL */
	if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0 ) {
		fprintf(stderr,
			"Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);			/* Clean up on exit */

	desired_bpp=8;
	/* Initialize the display */
	screen = SDL_SetVideoMode(w, h, desired_bpp, video_flags);
	if ( screen == NULL ) {
		fprintf(stderr, "Couldn't set %dx%dx%d video mode: %s\n",
					w, h, desired_bpp, SDL_GetError());
		exit(1);
	}

	/* Show some info */
	printf("Set %dx%dx%d mode\n",
			screen->w, screen->h, screen->format->BitsPerPixel);
	printf("Video surface located in %s memory.\n",
			(screen->flags&SDL_HWSURFACE) ? "video" : "system");
	
	/* Check for double buffering */
	if ( screen->flags & SDL_DOUBLEBUF ) {
		printf("Double-buffering enabled - good!\n");
	}

	/* Set the window manager title bar */
	SDL_WM_SetCaption("TestFonts", "testfonts");

	/* Do all the drawing work */
	Draw (screen);
	
	return(0);
}
