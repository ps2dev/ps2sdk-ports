/*
    SMPEG - SDL MPEG Player Library
    Copyright (C) 1999  Loki Entertainment Software

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
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <stdlib.h>
#include <string.h>
#include "SDL.h"
#include "MPEGfilter.h"


/* General filter cleanup function */
static void filter_destroy(SMPEG_Filter *filter)
{
    if ( filter ) {
        if ( filter->data ) {
            free(filter->data);
        }
        free(filter);
    }
}

/**************************************************/
/* The null filter. Copies source rect to overlay */
/**************************************************/

static void filter_null_callback(SDL_Overlay * dst, SDL_Overlay * src, SDL_Rect * region, SMPEG_FilterInfo * info, void * data)
{
  register Uint32 y;
  register Uint8 * s, * d;

  /* Y component */
  s = src->pixels[0];
  d = dst->pixels[0];
  /* Go to the top left corner of the source rectangle */
  s += region->y * src->pitches[0] + region->x; 
  for(y = 0; y < region->h; y++)
  {
    /* Copy lines */
    memcpy(d, s, region->w);   
    s += src->pitches[0];
    d += dst->pitches[0];
  }

  /* V component */
  s = src->pixels[1];
  d = dst->pixels[1];
  s += (region->y >> 1) * src->pitches[1] + (region->x >> 1);
  for(y = 0; y < region->h; y+=2)
  {
    memcpy(d, s, region->w >> 1);
    s += src->pitches[1];
    d += dst->pitches[1];
  }
  
  /* U component */
  s = src->pixels[2];
  d = dst->pixels[2];
  s += (region->y >> 1) * src->pitches[2] + (region->x >> 1);
  for(y = 0; y < region->h; y+=2)
  {
    memcpy(d, s, region->w >> 1);
    s += src->pitches[2];
    d += dst->pitches[2];
  }
}

SMPEG_Filter *SMPEGfilter_null(void)
{
    SMPEG_Filter *filter;

    filter = (SMPEG_Filter *)malloc(sizeof(*filter));
    if ( filter ) {
        /* the null filter requires no extra info */
        filter->flags = 0;
        /* no private data */
        filter->data = 0;
        /* set the function pointers to the callback and destroy functions */
        filter->callback = filter_null_callback;
        filter->destroy = filter_destroy;
    }
    return filter;
}

/************************************************************************************/
/* The bilinear filter. A basic low-pass filter that will produce a smoother image. */
/* It uses the following convolution matrix:         [0 1 0]                        */
/*                                                   [1 4 1]                        */
/*                                                   [0 1 0]                        */
/************************************************************************************/

static void filter_bilinear_callback(SDL_Overlay * dst, SDL_Overlay * src, SDL_Rect * region, SMPEG_FilterInfo * info, void * data)
{
  register int x, y;
  register Uint8 * s, * d;

  s = src->pixels[0];
  d = dst->pixels[0];

  s += region->y * src->pitches[0] + region->x;

  /* Skip first line */
  memcpy(d, s, region->w);
  d += dst->pitches[0];
  s += src->pitches[0];

  for(y = 1; y < region->h - 1; y++)
  {
    /* Skip first pixel */
    *d++ = *s++;

    for(x = 1; x < region->w - 1; x++)
    {
	*d++ = 
	  (((*s) << 2)           +       // 4*(x,y)   +
	   *(s - src->pitches[0])  +     //   (x,y-1) +
	   *(s - 1)              +       //   (x-1,y) +
	   *(s + 1)              +       //   (x+1,y) +
	   *(s + src->pitches[0]))       //   (x,y+1)
	  >> 3;                          // / 8
	s++;
    }

    /* Skip last pixel */
    *d++ = *s++;

    /* Go to next line */
    d += dst->pitches[0] - region->w;
    s += src->pitches[0] - region->w;
  }

  /* Skip last line */
  memcpy(d, s, region->w);
  d += dst->pitches[0];
  s += src->pitches[0];

  /* V component (unfiltered) */
  s = src->pixels[1];
  d = dst->pixels[1];
  s += (region->y >> 1) * src->pitches[1] + (region->x >> 1);
  for(y = 0; y < region->h; y+=2)
  {
    memcpy(d, s, region->w >> 1);
    s += src->pitches[1];
    d += dst->pitches[1];
  }
  
  /* U component (unfiltered) */
  s = src->pixels[2];
  d = dst->pixels[2];
  s += (region->y >> 1) * src->pitches[2] + (region->x >> 1);
  for(y = 0; y < region->h; y+=2)
  {
    memcpy(d, s, region->w >> 1);
    s += src->pitches[2];
    d += dst->pitches[2];
  }
}

SMPEG_Filter *SMPEGfilter_bilinear(void)
{
    SMPEG_Filter *filter;

    filter = (SMPEG_Filter *)malloc(sizeof(*filter));
    if ( filter ) {
        /* the bilinear filter requires no extra info */
        filter->flags = 0;
        /* no private data */
        filter->data = 0;
        /* set the function pointers to the callback and destroy functions */
        filter->callback = filter_bilinear_callback;
        filter->destroy = filter_destroy;
    }
    return filter;
}


/***************************************************************************************************/
/* The deblocking filter. It filters block borders and non-intra coded blocks to reduce blockiness */ 
/***************************************************************************************************/

static void filter_deblocking_callback(SDL_Overlay * dst, SDL_Overlay * src, SDL_Rect * region, SMPEG_FilterInfo * info, void * data)
{
  int x, y;
  Uint32 dL, dU, dR, dD;
  Uint32 aL, aU, aR, aD;
  Uint32 Q;
  Uint16 * coeffs;
  register Uint8 * s, * d;

  /* retrieve the coeffs from private data */
  coeffs = (Uint16 *) data;

  /* Y component */
  s = src->pixels[0];
  d = dst->pixels[0];

  s += region->y * src->pitches[0] + region->x;

  /* Skip first line */
  memcpy(d, s, region->w);
  d += dst->pitches[0];
  s += src->pitches[0];

  for(y = 1; y < region->h - 1; y++)
  {
    /* Skip first pixel */
    *d++ = *s++;

    for(x = 1; x < region->w - 1; x++)
    {
      /* get current block quantization error from the info structure provided by the video decoder */
      Q = info->yuv_mb_square_error[((region->y + y) >> 4) * (src->w >> 4) + ((region->x + x) >> 4)];

      if(!Q)
	*d++ = *s++; /* block is intra coded, don't filter */
      else
      {
	/* compute differences with up, left, right and down neighbors */
	dL = *s - *(s - 1) + 256;
	dR = *s - *(s + 1) + 256;
	dU = *s - *(s - src->pitches[0]) + 256;
	dD = *s - *(s + src->pitches[0]) + 256;

	/* get the corresponding filter coefficients from the lookup table */
	aU = coeffs[(y & 7) + (Q << 12) + (dU << 3)];
	aD = coeffs[(y & 7) + (Q << 12) + (dD << 3)];
	aL = coeffs[(x & 7) + (Q << 12) + (dL << 3)];
	aR = coeffs[(x & 7) + (Q << 12) + (dR << 3)];

	/* apply the filter on current pixel */
	*d++ = 
	  ((*s)*(4 * 65536 - aL - aR - aU - aD) + // (4-aL-aR-aU-aD)*(x,y) +
	   (*(s - src->pitches[0]))*aU +          // aU*(x,y-1) +
	   (*(s - 1))*aL +                        // aL*(x-1,y) +
	   (*(s + 1))*aR +                        // aR*(x+1,y) +
	   (*(s + src->pitches[0]))*aD)           // aU*(x,y+1)
	     >> 18;                               // remove fixed point and / 4
	s++;
      }
    }

    /* Skip last pixel */
    *d++ = *s++;

    /* Go to next line */
    d += dst->pitches[0] - region->w;
    s += src->pitches[0] - region->w;
  }

  /* Skip last line */
  memcpy(d, s, region->w);
  d += dst->pitches[0];
  s += src->pitches[0];

  /* V component (unfiltered) */
  s = src->pixels[1];
  d = dst->pixels[1];
  s += (region->y >> 1) * src->pitches[1] + (region->x >> 1);
  for(y = 0; y < region->h; y+=2)
  {
    memcpy(d, s, region->w >> 1);
    s += src->pitches[1];
    d += dst->pitches[1];
  }
  
  /* U component (unfiltered) */
  s = src->pixels[2];
  d = dst->pixels[2];
  s += (region->y >> 1) * src->pitches[2] + (region->x >> 1);
  for(y = 0; y < region->h; y+=2)
  {
    memcpy(d, s, region->w >> 1);
    s += src->pitches[2];
    d += dst->pitches[2];
  }
}

static void *allocate_deblocking_data(void)
{
  void * data;
  Uint16 * c;
  Uint32 q, q1, q5, q9, d;

  /* precalc table is 256Kb long */
  data = malloc(sizeof(*c)*8*32*512);
  c = (Uint16 *)data;

  /* precalc filter coefficients:                                     */
  /*                           1                                      */
  /* coeffs(Q,d,x,y) = _____________________                          */
  /*                           d(x,y)*d(x,y)                          */
  /*                   1  +    _____________                          */
  /*                             Q*Q*k(x,y)                           */
  /* where                                                            */
  /* k(x,y) = [ 9 9 9 9 9 9 9 9 ]                                     */
  /*          [ 9 5 5 5 5 5 5 9 ]                                     */
  /*          [ 9 5 1 1 1 1 5 9 ]                                     */
  /*          [ 9 5 1 1 1 1 5 9 ]                                     */
  /*          [ 9 5 1 1 1 1 5 9 ]                                     */
  /*          [ 9 5 1 1 1 1 5 9 ]                                     */
  /*          [ 9 5 5 5 5 5 5 9 ]                                     */
  /*          [ 9 9 9 9 9 9 9 9 ]                                     */
  /* and Q is the quantization error for the block                    */
  /* and d is the difference between current pixel and neighbor pixel */
  /*                                                                  */
  /* this is for the math :), now some additional tricks:             */
  /*                                                                  */
  /* all coeffs are multiplied by 65536 for precision (fixed point)   */
  /* and d is translated from [-128,127] to [0,255] (array indexation)*/


  for(d = 0; d < 512*8; d++)
    *c++ = 0;

  for(q = 1; q < 32; q++)
  {
    q1 = q*q;
    q9 = (q1 << 3) + q1;
    q5 = (q1 << 2) + q1;
    
    for(d = 0; d < 256; d++)
    {
      *c++ = (Uint16) ((q9 << 16) / ((256 - d)*(256 - d) + q9));
      *c++ = (Uint16) ((q5 << 16) / ((256 - d)*(256 - d) + q5));
      *c++ = (Uint16) ((q1 << 16) / ((256 - d)*(256 - d) + q1));
      *c++ = (Uint16) ((q1 << 16) / ((256 - d)*(256 - d) + q1));
      *c++ = (Uint16) ((q1 << 16) / ((256 - d)*(256 - d) + q1));
      *c++ = (Uint16) ((q1 << 16) / ((256 - d)*(256 - d) + q1));
      *c++ = (Uint16) ((q5 << 16) / ((256 - d)*(256 - d) + q5));
      *c++ = (Uint16) ((q9 << 16) / ((256 - d)*(256 - d) + q9));
    }
    for(d = 0; d < 256; d++)
    {
      *c++ = (Uint16) ((q9 << 16) / (d*d + q9));
      *c++ = (Uint16) ((q5 << 16) / (d*d + q5));
      *c++ = (Uint16) ((q1 << 16) / (d*d + q1));
      *c++ = (Uint16) ((q1 << 16) / (d*d + q1));
      *c++ = (Uint16) ((q1 << 16) / (d*d + q1));
      *c++ = (Uint16) ((q1 << 16) / (d*d + q1));
      *c++ = (Uint16) ((q5 << 16) / (d*d + q5));
      *c++ = (Uint16) ((q9 << 16) / (d*d + q9));
    }
  }
  return data;
}

SMPEG_Filter *SMPEGfilter_deblocking(void)
{
    SMPEG_Filter *filter;

    filter = (SMPEG_Filter *)malloc(sizeof(*filter));
    if ( filter ) {
        /* Ask the video decoder to provide per-block quantization error */
        filter->flags = SMPEG_FILTER_INFO_MB_ERROR;
        /* allocate private data */
        filter->data = allocate_deblocking_data();
        if ( ! filter->data ) {
            free(filter);
            return (SMPEG_Filter *)0;
        }
        /* set the function pointers to the callback and destroy functions */
        filter->callback = filter_deblocking_callback;
        filter->destroy = filter_destroy;
    }
    return filter;
}
