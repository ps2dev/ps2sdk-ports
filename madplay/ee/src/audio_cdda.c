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
 * $Id: audio_cdda.c,v 1.9 2004/01/23 09:41:31 rob Exp $
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "global.h"

# include <stdio.h>
# include <string.h>

# ifdef HAVE_ASSERT_H
#  include <assert.h>
# endif

# include <mad.h>

# include "audio.h"

static FILE *outfile;
unsigned int samplecount;

# define CD_FRAMESZ  (44100 / 75)

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

  samplecount = 0;

  return 0;
}

static
int config(struct audio_config *config)
{
  /* force 16-bit 44100 Hz stereo */

  config->channels  = 2;
  config->speed     = 44100;
  config->precision = 16;

  return 0;
}

static
int output(unsigned char const *data, unsigned int nsamples)
{
  unsigned int count;
  int result = 0;

  count = fwrite(data, 2 * 2, nsamples, outfile);

  if (count != nsamples) {
    audio_error = ":fwrite";
    result = -1;
  }

  samplecount = (samplecount + count) % CD_FRAMESZ;

  return result;
}

static
int play(struct audio_play *play)
{
  unsigned char data[MAX_NSAMPLES * 2 * 2];

  assert(play->samples[1]);

  audio_pcm_s16be(data, play->nsamples, play->samples[0], play->samples[1],
		  play->mode, play->stats);

  return output(data, play->nsamples);
}

static
int stop(struct audio_stop *stop)
{
  return 0;
}

static
int finish(struct audio_finish *finish)
{
  int result = 0;

  /* pad audio to CD frame boundary */

  if (samplecount) {
    unsigned char padding[CD_FRAMESZ * 2 * 2];
    unsigned int padsz;

    assert(samplecount < CD_FRAMESZ);
    padsz = CD_FRAMESZ - samplecount;

    memset(padding, 0, padsz * 2 * 2);

    result = output(padding, padsz);
  }

  if (outfile != stdout &&
      fclose(outfile) == EOF &&
      result == 0) {
    audio_error = ":fclose";
    result = -1;
  }

  return result;
}

int audio_cdda(union audio_control *control)
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
