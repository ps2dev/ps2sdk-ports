/*
 * MODIFIED by Bruce Merry (bmerry@iafrica.com) on 10/11/2000
 * - fixed to handle arbitrary tile dimensions, not just 1x2
 * - max_texture_size renamed to texture_size and used as a variable
 *   (to change the max texture size, edit the assigned value in the decl)
 * - hardcoded 256's changed to texture_size to allow small texture sizes
 *   (e.g. for very low-res movies)
 * - all pieces of movie copied into the top left corner of texture tiles,
 *   instead of being offset
 * - mechanism for keeping tiles aligned changed: a one texel border is
 *   included in the tiles, which I think is used by the filtering even though
 *   it is not explicitly selected for rendering (I think - I don't know much
 *   about OpenGL, I've just fiddled until it looked right)
 * - removed glmovie_is_power_of_2: it was not needed and
 *   it only went up to 2048 anyway.
 */

#include "glmovie.h"
#include <stdlib.h>
#include <string.h>

/* Some data is redundant at this stage. */
typedef struct glmovie_texture_t {
    GLuint id;           /* OpenGL texture id. */
    GLuint poly_width;   /* Quad width for tile. */
    GLuint poly_height;  /* Quad height for tile. */
    GLuint movie_width;  /* Width of movie inside tile. */
    GLuint movie_height; /* Height of movie inside tile. */
    GLuint skip_rows;    /* Number of rows of movie to skip */
    GLuint skip_pixels;  /* Number of columns of movie to skip */
    GLuint row;          /* Row number of tile in scheme. */
    GLuint col;          /* Column number of tile in scheme. */
} glmovie_texture;

/* Boy, is this not thread safe. */

/* Our evil maximum texture size. Boo 3Dfx! */
static GLuint texture_size = 256;

/* Keep this around for easy freeing later. */
static GLuint* texture_ids = NULL;
/* Our main data. */
static glmovie_texture* textures = NULL;
static GLuint num_texture_rows = 0;
static GLuint num_texture_cols = 0;
/* Width and height of all tiling. */
static GLuint tiled_width = 0;
static GLuint tiled_height = 0;
/* Width and height of entire movie. */
static GLuint movie_width = 0;
static GLuint movie_height = 0;

/*
 * Draw the frame data.
 *
 * Parameters:
 *    frame: Actual RGBA frame data
 */
void glmovie_draw( GLubyte* frame )
{
    GLuint i;
    GLdouble shift;

    glClear( GL_COLOR_BUFFER_BIT );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity( );

    shift = 1 / ((double) texture_size);
    for (i = 0; i < num_texture_rows * num_texture_cols; i++) {
        glBindTexture( GL_TEXTURE_2D, textures[i].id );
        glPixelStorei( GL_UNPACK_ROW_LENGTH, movie_width );
        glPixelStorei( GL_UNPACK_SKIP_ROWS, textures[i].skip_rows );
        glPixelStorei( GL_UNPACK_SKIP_PIXELS, textures[i].skip_pixels );

        glTexSubImage2D( GL_TEXTURE_2D,
                         0,
                         0,                       /* offset_x */
                         0,                       /* offset_y */
                         textures[i].movie_width + 2,
                         textures[i].movie_height + 2,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         frame );

        glBegin( GL_QUADS );
        glTexCoord2f( shift, shift );
        glVertex2i( textures[i].col * texture_size,
                    textures[i].row * texture_size );
        glTexCoord2f( shift, shift + (textures[i].movie_height)/((double) texture_size) );
        glVertex2i( textures[i].col * texture_size,
                    (textures[i].row + 1) * texture_size);
        glTexCoord2f( shift + (textures[i].movie_width)/((double) texture_size),
                      shift + (textures[i].movie_height)/((double) texture_size) );
        glVertex2i( (textures[i].col + 1) * texture_size,
                    (textures[i].row + 1) * texture_size);
        glTexCoord2f( shift + (textures[i].movie_width)/((double) texture_size), shift );
        glVertex2i( (textures[i].col + 1) * texture_size,
                    textures[i].row * texture_size );
        glEnd( );
    }
}

/*
 * Here we need to center the OpenGL viewport within the
 * window size that we are given.
 *
 * Parameters:
 *     width: Width of the window in pixels
 *     height: Height of the window in pixels
 */
void glmovie_resize( GLuint width, GLuint height )
{
    glViewport( 0, 0, width, height );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );
    gluOrtho2D( 0, tiled_width, tiled_height, 0 );
}

/*
 * Calculates the next power of 2 given a particular value.
 * Useful for calculating proper texture sizes for non power-of-2
 * aligned texures.
 *
 * Parameters:
 *     seed: Value to begin from
 * Returns:
 *     Next power of 2 beginning from 'seed'
 */
GLuint glmovie_next_power_of_2( GLuint seed )
{
    GLuint i;

    for( i = 1; i < seed; i *= 2 ) { };

    return i;
}

/*
 * Initialize the movie player subsystem with the width and height
 * of the *movie data* (as opposed to the window).
 *
 * Parameters:
 *     width: Width of movie in pixels
 *     height: Height of movie in pixels
 * Returns:
 *     GL_NO_ERROR on success
 *     Any of the enumerated GL errors on failure
 */
GLenum glmovie_init( GLuint width, GLuint height )
{
    /* Initial black texels. */
    GLubyte* pixels;
    /* Absolute offsets from within tiled frame. */
    /* GLuint offset_x = 0; */
    /* GLuint offset_y = 0; */
    GLuint skip_rows = 0;
    GLuint skip_pixels = 0;
    GLuint i, j, current;

    /* Save original movie dimensions. */
    movie_width = width;
    movie_height = height;

    /* Get the power of 2 dimensions. */
    tiled_width = glmovie_next_power_of_2( width );
    tiled_height = glmovie_next_power_of_2( height );
    while ( texture_size > tiled_width || texture_size > tiled_height )
        texture_size /= 2;

    /* Now break it up into quads. */
    num_texture_rows = tiled_height / texture_size;
    num_texture_cols = tiled_width / texture_size;

    /* Time for fun with data structures. */
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    glEnable( GL_TEXTURE_2D );
    glEnable( GL_DITHER );
    texture_ids = (GLuint*) malloc( sizeof( GLuint ) * num_texture_rows * num_texture_cols );

    if( !texture_ids ) {
        return GL_OUT_OF_MEMORY;
    }

    glGenTextures( num_texture_rows * num_texture_cols, texture_ids );

    textures = (glmovie_texture*) malloc( sizeof( glmovie_texture ) *
                                          num_texture_rows * num_texture_cols );

    if( !textures ) {
        glDeleteTextures( num_texture_rows * num_texture_cols, texture_ids );
        free( texture_ids );
        return GL_OUT_OF_MEMORY;
    }

    for ( i = 0, current = 0; i < num_texture_rows; i++ ) {
        skip_pixels = 0;
        for ( j = 0; j < num_texture_cols; j++, current++ ) {
            /* Setup texture. */
            textures[current].id = texture_ids[current];
            textures[current].poly_width = texture_size;
            textures[current].poly_height = texture_size;
            textures[current].movie_width =
                (movie_width - 2) * (j + 1) / num_texture_cols - skip_pixels;
            textures[current].movie_height =
                (movie_height - 2) * (i + 1) / num_texture_rows - skip_rows;
            textures[current].row = i;
            textures[current].col = j;
            textures[current].skip_pixels = skip_pixels;
            textures[current].skip_rows = skip_rows;
            skip_pixels += textures[current].movie_width;

            pixels = (GLubyte*) malloc( textures[current].poly_width * textures[current].poly_height * 4 );
            memset( pixels, 0, textures[current].poly_width * textures[current].poly_height * 4 );

            if( !pixels ) {
                glDeleteTextures( num_texture_rows * num_texture_cols, texture_ids );
                free( texture_ids );
                free( textures );
                return GL_OUT_OF_MEMORY;
            }


            /* Do all of our useful binding. */
            glBindTexture( GL_TEXTURE_2D, textures[current].id );
            glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

            /* Specify our 256x256 black texture. */
            glTexImage2D( GL_TEXTURE_2D,
                          0,
                          GL_RGB,
                          textures[current].poly_width,
                          textures[current].poly_height,
                          0,
                          GL_RGBA,
                          GL_UNSIGNED_BYTE,
                          pixels );

            free( pixels );
        }
        skip_rows += textures[current].movie_height;
    }

    /* Simple state setup at the end. */
    glClearColor( 0.0, 0.0, 0.0, 0.0 );

    return glGetError( );
}

/*
 * Free any resources associated with the movie player.
 */
void glmovie_quit( void )
{
    glDeleteTextures( num_texture_rows * num_texture_cols, texture_ids );
    free( texture_ids );
    free( textures );
}
