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

# include <stdio.h>
# include <string.h>
# include <mad.h>

# include "audio.h"

# if defined(WORDS_BIGENDIAN)
#  define audio_pcm_s32  audio_pcm_s32be
#  define audio_pcm_s24  audio_pcm_s24be
#  define audio_pcm_s16  audio_pcm_s16be
# else
#  define audio_pcm_s32  audio_pcm_s32le
#  define audio_pcm_s24  audio_pcm_s24le
#  define audio_pcm_s16  audio_pcm_s16le
# endif

static FILE *outfile;
static audio_pcmfunc_t *audio_pcm;

static
int init(struct audio_init *init)
{
  if (init->path && strcmp(init->path, "-") != 0) {
    outfile = fopen(init->path, "wb");
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
  unsigned int bitdepth;

  bitdepth = config->precision & ~7;
  if (bitdepth == 0)
    bitdepth = 16;
  else if (bitdepth > 32)
    bitdepth = 32;

  switch (config->precision = bitdepth) {
  case 8:
    audio_pcm = audio_pcm_u8;
    break;

  case 16:
    audio_pcm = audio_pcm_s16;
    break;

  case 24:
    audio_pcm = audio_pcm_s24;
    break;

  case 32:
    audio_pcm = audio_pcm_s32;
    break;
  }

  return 0;
}

static
int play(struct audio_play *play)
{
  unsigned char data[MAX_NSAMPLES * 4 * 2];
  unsigned int len;

  len = audio_pcm(data, play->nsamples, play->samples[0], play->samples[1],
		  play->mode, play->stats);

  if (fwrite(data, len, 1, outfile) != 1) {
    audio_error = ":fwrite";
    return -1;
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

int audio_raw(union audio_control *control)
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
