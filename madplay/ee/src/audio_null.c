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

# include <mad.h>

# include "audio.h"

static
int init(struct audio_init *init)
{
  return 0;
}

static
int config(struct audio_config *config)
{
  return 0;
}

static
void update_stats(struct audio_stats *stats,
		  unsigned int nsamples, mad_fixed_t const *sample)
{
  enum {
    MIN = -MAD_F_ONE,
    MAX =  MAD_F_ONE - 1
  };

  while (nsamples--) {
    if (*sample >= stats->peak_sample) {
      stats->peak_sample = *sample;

      if (*sample > MAX && *sample - MAX > stats->peak_clipping)
	stats->peak_clipping = *sample - MAX;
    }
    else if (*sample < -stats->peak_sample) {
      stats->peak_sample = -*sample;

      if (*sample < MIN && MIN - *sample > stats->peak_clipping)
	stats->peak_clipping = MIN - *sample;
    }

    ++sample;
  }
}

static
int play(struct audio_play *play)
{
  update_stats(play->stats, play->nsamples, play->samples[0]);
  if (play->samples[1])
    update_stats(play->stats, play->nsamples, play->samples[1]);

  return 0;
}

static
int stop(struct audio_stop *stop)
{
  return 0;
}

static
int finish(struct audio_finish *finish)
{
  return 0;
}

int audio_null(union audio_control *control)
{
  audio_error = 0;

  switch (control->command) {
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
