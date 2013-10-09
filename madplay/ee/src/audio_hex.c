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
 * $Id: audio_hex.c,v 1.23 2004/01/23 09:41:31 rob Exp $
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "global.h"

# include <stdio.h>
# include <string.h>
# include <mad.h>

# include "audio.h"

static FILE *outfile;
static unsigned int bitdepth;
static char format_str[7];

static
int init(struct audio_init *init)
{
  if (init->path && strcmp(init->path, "-") != 0) {
    outfile = fopen(init->path, "w");
    if (outfile == 0) {
      audio_error = ":";
      return -1;
    }
  }
  else
    outfile = stdout;

  return 0;
}

static
int config(struct audio_config *config)
{
  bitdepth = config->precision & ~3;
  if (bitdepth == 0 || bitdepth > 24)
    bitdepth = 24;

  config->precision = bitdepth;

  sprintf(format_str, "%%0%1ulX\n", bitdepth / 4);

  fprintf(outfile, "# %u channel%s, %u Hz, %u-bit samples\n",
	  config->channels, config->channels == 1 ? "" : "s",
	  config->speed, config->precision);

  return 0;
}

static
int play(struct audio_play *play)
{
  unsigned int len;
  mad_fixed_t const *left, *right;
  unsigned long mask;

  len   = play->nsamples;
  left  = play->samples[0];
  right = play->samples[1];

  mask = (1L << bitdepth) - 1;

  switch (play->mode) {
  case AUDIO_MODE_ROUND:
    while (len--) {
      fprintf(outfile, format_str,
	      audio_linear_round(bitdepth, *left++, play->stats) & mask);

      if (right) {
	fprintf(outfile, format_str,
		audio_linear_round(bitdepth, *right++, play->stats) & mask);
      }
    }
    break;

  case AUDIO_MODE_DITHER:
    {
      static struct audio_dither left_dither, right_dither;

      while (len--) {
	fprintf(outfile, format_str,
		audio_linear_dither(bitdepth, *left++, &left_dither,
				    play->stats) & mask);

	if (right) {
	  fprintf(outfile, format_str,
		  audio_linear_dither(bitdepth, *right++, &right_dither,
				      play->stats) & mask);
	}
      }
    }
    break;
  }

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
  if (outfile != stdout &&
      fclose(outfile) == EOF) {
    audio_error = ":fclose";
    return -1;
  }

  return 0;
}

int audio_hex(union audio_control *control)
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
