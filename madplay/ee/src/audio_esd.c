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

# include <unistd.h>
# include <errno.h>
# include <esd.h>
# include <mad.h>

# include "gettext.h"

# include "audio.h"

# if defined(WORDS_BIGENDIAN)
#  define audio_pcm_s16  audio_pcm_s16be
# else
#  define audio_pcm_s16  audio_pcm_s16le
# endif

static char const *host;
static int esd;
static audio_pcmfunc_t *audio_pcm;

static
int init(struct audio_init *init)
{
  host = init->path;
  if (host && *host == 0)
    host = 0;

  /* test opening a socket */

  esd = esd_open_sound(host);
  if (esd < 0) {
    audio_error = _("esd_open_sound() failed");
    return -1;
  }

  return 0;
}

static
int config(struct audio_config *config)
{
  unsigned int bitdepth;
  esd_format_t format = ESD_STREAM | ESD_PLAY;
  extern char const *argv0;
  int result = 0;

  bitdepth = config->precision & ~7;
  if (bitdepth == 0 || bitdepth > 16)
    bitdepth = 16;

  switch (config->precision = bitdepth) {
  case 8:
    audio_pcm = audio_pcm_u8;
    format |= ESD_BITS8;
    break;

  case 16:
    audio_pcm = audio_pcm_s16;
    format |= ESD_BITS16;
    break;
  }

  format |= (config->channels == 2) ? ESD_STEREO : ESD_MONO;

  if (esd_close(esd) < 0) {
    audio_error = ":esd_close";
    result = -1;
  }

  esd = esd_play_stream_fallback(format, config->speed, host, argv0);
  if (esd < 0 && result == 0) {
    audio_error = _("esd_play_stream_fallback() failed");
    result = -1;
  }

  return result;
}

static
int output(unsigned char const *ptr, unsigned int len)
{
  while (len) {
    int wrote;

    wrote = write(esd, ptr, len);
    if (wrote == -1) {
      if (errno == EINTR)
	continue;
      else {
	audio_error = ":write";
	return -1;
      }
    }

    ptr += wrote;
    len -= wrote;
  }

  return 0;
}

static
int play(struct audio_play *play)
{
  unsigned char data[MAX_NSAMPLES * 2 * 2];
  unsigned int len;

  len = audio_pcm(data, play->nsamples, play->samples[0], play->samples[1],
		  play->mode, play->stats);

  return output(data, len);
}

static
int stop(struct audio_stop *stop)
{
  return 0;
}

static
int finish(struct audio_finish *finish)
{
  if (esd_close(esd) < 0) {
    audio_error = ":esd_close";
    return -1;
  }

  return 0;
}

int audio_esd(union audio_control *control)
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
