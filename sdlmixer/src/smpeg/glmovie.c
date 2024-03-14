/* HACK
 * If you stick glx.h before MPEG.h, the preprocessor
 * will start replacing the MPEG methods Status with an
 * X11 variable type... blech.
 */
#include "smpeg.h"
#include "SDL.h"
#include <stdlib.h>
#include <string.h>
/*#include <unistd.h>*/
#include "glmovie.h"

static void glmpeg_update( SDL_Surface*, Sint32, Sint32, Uint32, Uint32 );

int main( int argc, char* argv[] )
{
    SMPEG* mpeg;
    SMPEG_Info mpeg_info;
    SDL_Surface* screen;
    SDL_Surface* surface;

    if( argc < 2 ) {
	fprintf( stderr, "Usage: %s file.mpg\n", argv[0]);
	return 1;
    }

    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 ) {
	fprintf( stderr, "glmovie: I couldn't initizlize SDL (shrug)\n" );
	return 1;
    }

    mpeg = SMPEG_new( argv[1], &mpeg_info, 1 );
    if( !mpeg ) {
	fprintf( stderr, "glmovie: I'm not so sure about this %s file...\n", argv[1] );
        SDL_Quit();
	return 1;
    }

    /* Grab the mouse and input and set the video mode */
    SDL_ShowCursor(0);
    //SDL_WM_GrabInput(SDL_GRAB_ON);
    screen = SDL_SetVideoMode(640, 480, 0, SDL_OPENGL); //|SDL_FULLSCREEN);
    if ( !screen ) {
	fprintf( stderr, "glmovie: Couldn't set 640x480 GL video mode: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    /* Everything needs to be in RGB for GL, but needs to be 32-bit for SMPEG. */
    surface = SDL_AllocSurface( SDL_SWSURFACE,
				mpeg_info.width,
				mpeg_info.height,
				32,
				0x000000FF,
				0x0000FF00,
				0x00FF0000,
				0xFF000000 );

    if( !surface ) {
	fprintf( stderr, "glmovie: I couldn't make a surface (boo hoo)\n" );
        SDL_Quit();
	exit( 1 );
    }

    /* *Initialize* with mpeg size. */
    if ( glmovie_init( mpeg_info.width, mpeg_info.height ) != GL_NO_ERROR ) {
	fprintf( stderr, "glmovie: glmovie_init() failed!\n" );
        SDL_Quit();
	exit( 1 );
    }

    /* *Resize* with window size. */
    glmovie_resize( screen->w, screen->h );
    SMPEG_setdisplay( mpeg, surface, NULL, glmpeg_update );
    SMPEG_play( mpeg );

    while( SMPEG_status( mpeg ) == SMPEG_PLAYING ) {
        SDL_Event event;

        while ( SDL_PollEvent(&event) ) {
            switch (event.type) {
              case SDL_KEYDOWN:
                if ( event.key.keysym.sym == SDLK_ESCAPE ) {
                    SMPEG_stop( mpeg );
                }
                break;
              case SDL_MOUSEBUTTONDOWN:
              case SDL_QUIT:
                SMPEG_stop( mpeg );
                break;
            }
        }
        SDL_Delay(100);
    }

    glmovie_quit( );

    SDL_Quit();
    return 0;
}

static void glmpeg_update( SDL_Surface* surface, Sint32 x, Sint32 y, Uint32 w, Uint32 h )
{
    GLenum error;

    if (( !surface ) || ( !surface->pixels )) {
        fprintf(stderr, "\n\nERROR: There's no surface for drawing?!\n\n");
        exit(1);
    }

    glmovie_draw( (GLubyte*) surface->pixels );

    error = glGetError( );

    if( error != GL_NO_ERROR ) {
	fprintf( stderr, "glmovie: GL error: %s\n", gluErrorString( error ) );
	exit( 1 );
    }

    SDL_GL_SwapBuffers();
}

