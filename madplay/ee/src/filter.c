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

# include <stdlib.h>
# include <stdio.h>
# include <mad.h>

# include "filter.h"
# include "player.h"

/*
 * NAME:	filter->init()
 * DESCRIPTION:	initialize a new filter
 */
void filter_init(struct filter *filter,
		 filter_func_t *func, void *data, struct filter *chain)
{
  filter->flags = 0;
  filter->func  = func;
  filter->data  = data;
  filter->chain = chain;
}

/*
 * NAME:	filter->new()
 * DESCRIPTION:	allocate a new filter object and chain it to another
 */
struct filter *filter_new(filter_func_t *func, void *data,
			  struct filter *chain)
{
  struct filter *filter;

  filter = malloc(sizeof(*filter));
  if (filter) {
    filter_init(filter, func, data, chain);
    filter->flags |= FILTER_FLAG_DMEM;
  }

  return filter;
}

/*
 * NAME:	filter->free()
 * DESCRIPTION:	deallocate a filter object chain
 */
void filter_free(struct filter *filter)
{
  while (filter) {
    struct filter *chain;

    chain = filter->chain;

    if (filter->flags & FILTER_FLAG_DMEM)
      free(filter);
    else
      filter->chain = 0;

    filter = chain;
  }
}

/*
 * NAME:	filter->run()
 * DESCRIPTION:	execute a filter chain
 */
enum mad_flow filter_run(struct filter *filter, struct mad_frame *frame)
{
  while (filter) {
    enum mad_flow result;

    result = filter->func(filter->data, frame);
    if (result != MAD_FLOW_CONTINUE)
      return result;

    filter = filter->chain;
  }

  return MAD_FLOW_CONTINUE;
}

/* --- Filters ------------------------------------------------------------- */

/*
 * NAME:	gain_filter()
 * DESCRIPTION:	perform attenuation or amplification
 */
enum mad_flow gain_filter(void *data, struct mad_frame *frame)
{
  mad_fixed_t gain = *(mad_fixed_t *) data;

  if (gain != MAD_F_ONE) {
    unsigned int nch, ch, ns, s, sb;

    nch = MAD_NCHANNELS(&frame->header);
    ns  = MAD_NSBSAMPLES(&frame->header);

    for (ch = 0; ch < nch; ++ch) {
      for (s = 0; s < ns; ++s) {
	for (sb = 0; sb < 32; ++sb) {
	  frame->sbsample[ch][s][sb] =
	    mad_f_mul(frame->sbsample[ch][s][sb], gain);
	}
      }
    }
  }

  return MAD_FLOW_CONTINUE;
}

# if 0
/*
 * NAME:	limit_filter()
 * DESCRIPTION:	limiting filter
 */
enum mad_flow limit_filter(void *data, struct mad_frame *frame)
{
  struct player *player = data;
  unsigned int nch, ch, ns, s, sb;

  nch = MAD_NCHANNELS(&frame->header);
  ns  = MAD_NSBSAMPLES(&frame->header);

  for (ch = 0; ch < nch; ++ch) {
    for (s = 0; s < ns; ++s) {
      for (sb = 0; sb < 32; ++sb) {
	frame->sbsample[ch][s][sb];
      }
    }
  }

  return MAD_FLOW_CONTINUE;
}
# endif

/*
 * NAME:	mono_filter()
 * DESCRIPTION:	transform stereo frame to mono
 */
enum mad_flow mono_filter(void *data, struct mad_frame *frame)
{
  if (frame->header.mode != MAD_MODE_SINGLE_CHANNEL) {
    unsigned int ns, s, sb;
    mad_fixed_t left, right;

    ns = MAD_NSBSAMPLES(&frame->header);

    for (s = 0; s < ns; ++s) {
      for (sb = 0; sb < 32; ++sb) {
	left  = frame->sbsample[0][s][sb];
	right = frame->sbsample[1][s][sb];

	frame->sbsample[0][s][sb] = (left + right) / 2;
	/* frame->sbsample[1][s][sb] = 0; */
      }
    }

    frame->header.mode = MAD_MODE_SINGLE_CHANNEL;
  }

  return MAD_FLOW_CONTINUE;
}

/*
 * NAME:	fadein_filter()
 * DESCRIPTION:	fade-in filter
 */
enum mad_flow fadein_filter(void *data, struct mad_frame *frame)
{
  struct player *player = data;

  if (mad_timer_compare(player->stats.play_timer, player->fade_in) < 0) {
    mad_timer_t frame_start, frame_end, ratio;
    unsigned int nch, nsamples, s;
    mad_fixed_t step, scalefactor;

    /*
     * Fade-in processing may occur over the entire frame, or it may end
     * somewhere within the frame. Find out where processing should end.
     */

    nsamples = MAD_NSBSAMPLES(&frame->header);

    /* this frame has not yet been added to play_timer */

    frame_start = frame_end = player->stats.play_timer;
    mad_timer_add(&frame_end, frame->header.duration);

    if (mad_timer_compare(player->fade_in, frame_end) < 0) {
      mad_timer_t length;

      length = frame_start;

      mad_timer_negate(&length);
      mad_timer_add(&length, player->fade_in);

      mad_timer_set(&ratio, 0,
		    mad_timer_count(length, frame->header.samplerate),
		    mad_timer_count(frame->header.duration,
				    frame->header.samplerate));

      nsamples = mad_timer_fraction(ratio, nsamples);
    }

    /* determine starting scalefactor and step size */

    mad_timer_set(&ratio, 0,
		  mad_timer_count(frame_start, frame->header.samplerate),
		  mad_timer_count(player->fade_in, frame->header.samplerate));

    scalefactor = mad_timer_fraction(ratio, MAD_F_ONE);
    step = MAD_F_ONE / (mad_timer_count(player->fade_in,
					frame->header.samplerate) / 32);

    /* scale subband samples */

    nch = MAD_NCHANNELS(&frame->header);

    for (s = 0; s < nsamples; ++s) {
      unsigned int ch, sb;

      for (ch = 0; ch < nch; ++ch) {
	for (sb = 0; sb < 32; ++sb) {
	  frame->sbsample[ch][s][sb] =
	    mad_f_mul(frame->sbsample[ch][s][sb], scalefactor);
	}
      }

      scalefactor += step;
    }
  }

  return MAD_FLOW_CONTINUE;
}

# if defined(EXPERIMENTAL)
/*
 * NAME:	mixer_filter()
 * DESCRIPTION: pre-empt decoding by dumping frame to independent mixer
 */
enum mad_flow mixer_filter(void *data, struct mad_frame *frame)
{
  FILE *dest = data;

  if (fwrite(frame, sizeof(*frame), 1, dest) != 1)
    return MAD_FLOW_BREAK;

  return MAD_FLOW_IGNORE;
}

/*
 * NAME:	experimental_filter()
 * DESCRIPTION:	experimental filter
 */
enum mad_flow experimental_filter(void *data, struct mad_frame *frame)
{
  if (frame->header.mode == MAD_MODE_STEREO ||
      frame->header.mode == MAD_MODE_JOINT_STEREO) {
    unsigned int ns, s, sb;

    ns = MAD_NSBSAMPLES(&frame->header);

    /* enhance stereo separation */

    for (s = 0; s < ns; ++s) {
      for (sb = 0; sb < 32; ++sb) {
	mad_fixed_t left, right;

	left  = frame->sbsample[0][s][sb];
	right = frame->sbsample[1][s][sb];

	frame->sbsample[0][s][sb] -= right / 4;
	frame->sbsample[1][s][sb] -= left  / 4;
      }
    }
  }

  return MAD_FLOW_CONTINUE;
}
# endif
