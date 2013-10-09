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
 * $Id: audio_oss.c,v 1.34 2004/01/23 09:41:31 rob Exp $
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "global.h"

# include <unistd.h>
# include <fcntl.h>
# include <sys/ioctl.h>

# if defined(HAVE_SYS_SOUNDCARD_H)
#  include <sys/soundcard.h>
# elif defined(HAVE_MACHINE_SOUNDCARD_H)
#  include <machine/soundcard.h>
# else
#  error "need <sys/soundcard.h> or <machine/soundcard.h>"
# endif

# include <errno.h>
# include <mad.h>

# include "gettext.h"

# include "audio.h"

# if !defined(AFMT_S32_NE)
#  if defined(WORDS_BIGENDIAN)
#   define AFMT_S32_NE  AFMT_S32_BE
#  else
#   define AFMT_S32_NE  AFMT_S32_LE
#  endif
# endif

# if !defined(AFMT_S16_NE)
#  if defined(WORDS_BIGENDIAN)
#   define AFMT_S16_NE  AFMT_S16_BE
#  else
#   define AFMT_S16_NE  AFMT_S16_LE
#  endif
# endif

# undef AUDIO_TRY32BITS
# if (defined(AFMT_S32_LE) && AFMT_S32_NE == AFMT_S32_LE) ||  \
     (defined(AFMT_S32_BE) && AFMT_S32_NE == AFMT_S32_BE)
#  define AUDIO_TRY32BITS
# endif

# if !defined(SNDCTL_DSP_CHANNELS) && defined(SOUND_PCM_WRITE_CHANNELS)
#  define SNDCTL_DSP_CHANNELS  SOUND_PCM_WRITE_CHANNELS
# endif

# if !defined(SNDCTL_DSP_SPEED) && defined(SOUND_PCM_WRITE_RATE)
#  define SNDCTL_DSP_SPEED  SOUND_PCM_WRITE_RATE
# endif

# define AUDIO_DEVICE1	"/dev/sound/dsp"
# define AUDIO_DEVICE2	"/dev/dsp"

static int sfd;
static audio_pcmfunc_t *audio_pcm;

static
int init(struct audio_init *init)
{
  if (init->path)
    sfd = open(init->path, O_WRONLY);
  else {
    sfd = open(init->path = AUDIO_DEVICE1, O_WRONLY);
    if (sfd == -1)
      sfd = open(init->path = AUDIO_DEVICE2, O_WRONLY);
  }

  if (sfd == -1) {
    audio_error = ":";
    return -1;
  }

  return 0;
}

static
int config(struct audio_config *config)
{
  unsigned int bitdepth;
  int format;

  bitdepth = config->precision & ~7;
# if defined(AUDIO_TRY32BITS)
  if (bitdepth == 0 || bitdepth > 32)
    bitdepth = 32;
# else
  if (bitdepth == 0 || bitdepth > 16)
    bitdepth = 16;
# endif

  if (ioctl(sfd, SNDCTL_DSP_SYNC, 0) == -1) {
    audio_error = ":ioctl(SNDCTL_DSP_SYNC)";
    return -1;
  }

  switch (bitdepth) {
  case 8:
    format = AFMT_U8;
    break;

  case 16:
    format = AFMT_S16_NE;
    break;

# if defined(AUDIO_TRY32BITS)
  case 24:
    bitdepth = 32;
  case 32:
    format = AFMT_S32_NE;
    break;
# endif
  }

  /* check supported formats */
  {
    int mask;

    if (ioctl(sfd, SNDCTL_DSP_GETFMTS, &mask) != -1) {
      while (!(mask & format) && format != AFMT_U8) {
	switch (format) {
# if defined(AUDIO_TRY32BITS)
	case AFMT_S32_LE:
	case AFMT_S32_BE:
	  bitdepth = 16;
	  format = AFMT_S16_NE;
	  break;
# endif

	case AFMT_S16_LE:
	case AFMT_S16_BE:
	  bitdepth = 8;
	  format = AFMT_U8;
	  break;
	}
      }
    }
  }

  while (ioctl(sfd, SNDCTL_DSP_SETFMT, &format) == -1) {
# if defined(AUDIO_TRY32BITS)
    /*
     * Some audio drivers may return an error instead of indicating a
     * supported format when 32-bit format is requested but not available.
     */
    if (bitdepth == 32) {
      bitdepth = 16;
      format = AFMT_S16_NE;
      continue;
    }
# endif

    audio_error = ":ioctl(SNDCTL_DSP_SETFMT)";
    return -1;
  }

  switch (format) {
# if defined(AFMT_S32_LE)
  case AFMT_S32_LE:
    audio_pcm = audio_pcm_s32le;
    bitdepth  = 32;
    break;
# endif

# if defined(AFMT_S32_BE)
  case AFMT_S32_BE:
    audio_pcm = audio_pcm_s32be;
    bitdepth  = 32;
    break;
# endif

  case AFMT_S16_LE:
    audio_pcm = audio_pcm_s16le;
    bitdepth  = 16;
    break;

  case AFMT_S16_BE:
    audio_pcm = audio_pcm_s16be;
    bitdepth  = 16;
    break;

  case AFMT_U8:
    audio_pcm = audio_pcm_u8;
    bitdepth  = 8;
    break;

  case AFMT_MU_LAW:
    audio_pcm = audio_pcm_mulaw;
    bitdepth  = 8;
    break;

  default:
    audio_error = _("no supported audio format available");
    return -1;
  }

  config->precision = bitdepth;

  if (ioctl(sfd, SNDCTL_DSP_CHANNELS, &config->channels) == -1) {
    audio_error = ":ioctl(SNDCTL_DSP_CHANNELS)";
    return -1;
  }

  if (ioctl(sfd, SNDCTL_DSP_SPEED, &config->speed) == -1) {
    audio_error = ":ioctl(SNDCTL_DSP_SPEED)";
    return -1;
  }

  return 0;
}

static
int output(unsigned char const *ptr, unsigned int len)
{
  while (len) {
    int wrote;

    wrote = write(sfd, ptr, len);
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
  unsigned char data[MAX_NSAMPLES * 4 * 2];
  unsigned int len;

  len = audio_pcm(data, play->nsamples, play->samples[0], play->samples[1],
		  play->mode, play->stats);

  return output(data, len);
}

static
int stop(struct audio_stop *stop)
{
  /* ignores stop->flush; no way to pause immediately without flushing? */

  if (ioctl(sfd, SNDCTL_DSP_RESET, 0) == -1) {
    audio_error = ":ioctl(SNDCTL_DSP_RESET)";
    return -1;
  }

  return 0;
}

static
int finish(struct audio_finish *finish)
{
  if (close(sfd) == -1) {
    audio_error = ":close";
    return -1;
  }

  return 0;
}

int audio_oss(union audio_control *control)
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
