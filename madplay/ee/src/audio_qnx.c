/*
 * madplay - MPEG audio decoder and player
 * Copyright (C) 2000-2004 Robert Leslie
 * QNX audio output module (C) 2001 Steven Grimm
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
# include <sys/asoundlib.h>
# include <stdlib.h>
# include <mad.h>

# include "gettext.h"

# include "audio.h"

# if defined(WORDS_BIGENDIAN)
#  define audio_pcm_s16  audio_pcm_s16be
# else
#  define audio_pcm_s16  audio_pcm_s16le
# endif

static snd_pcm_t *pcm_handle;
static snd_mixer_t *mixer_handle;
static int card = -1, dev = -1, frag_size = 8192, frags_max = 32;

static void fill_int(char *name, int *var)
{
  char *tmp = getenv(name);

  if (tmp != NULL)
    *var = atoi(tmp);
}

static
int init(struct audio_init *init)
{
  fill_int("MADPLAY_CARD", &card);
  fill_int("MADPLAY_DEV", &dev);
  fill_int("MADPLAY_FRAG_SIZE", &frag_size);
  fill_int("MADPLAY_FRAGS_MAX", &frags_max);

  if (card == -1)
  {
    if (snd_pcm_open_preferred(&pcm_handle, &card, &dev,
        SND_PCM_OPEN_PLAYBACK) < 0)
    {
      audio_error = _("can't open sound card");
      return -1;
    }
  }
  else
  {
    if (snd_pcm_open(&pcm_handle, card, dev,
        SND_PCM_OPEN_PLAYBACK) < 0)
    {
      audio_error = _("can't open sound card");
      return -1;
    }
  }

  if (snd_pcm_plugin_set_disable(pcm_handle, PLUGIN_DISABLE_MMAP) < 0)
  {
    audio_error = _("can't disable mmap mode");
    return -1;
  }

  return 0;
}

static
int config(struct audio_config *config)
{
  snd_pcm_channel_params_t pp;
  snd_pcm_channel_setup_t setup;
  snd_mixer_group_t group;

  memset(&pp, 0, sizeof(pp));
  memset(&setup, 0, sizeof(setup));
  memset(&group, 0, sizeof(group));

  pp.mode = SND_PCM_MODE_BLOCK;
  pp.channel = SND_PCM_CHANNEL_PLAYBACK;
  pp.start_mode = SND_PCM_START_FULL;
  pp.stop_mode = SND_PCM_STOP_STOP;


/*
  pp.buf.stream.queue_size = 8192;	// No docs on these fields!!!
  pp.buf.stream.fill = 6144;
  pp.buf.stream.max_fill = 8192;
*/
  pp.buf.block.frag_size = frag_size;
  pp.buf.block.frags_max = frags_max;
  pp.buf.block.frags_min = 1;

  pp.format.interleave = 1;
  pp.format.rate = config->speed;
  pp.format.voices = config->channels;
  pp.format.format = SND_PCM_SFMT_S16_LE;

  /* RSL: indicate 16-bit output */
  config->precision = 16;

  if (snd_pcm_plugin_params(pcm_handle, &pp) < 0)
  {
    audio_error = _("can't configure device");
    return -1;
  }

  if (snd_pcm_plugin_prepare (pcm_handle, SND_PCM_CHANNEL_PLAYBACK) < 0)
  {
    audio_error = _("can't prepare device");
    return -1;
  }

  setup.channel = SND_PCM_CHANNEL_PLAYBACK;
  setup.mixer_gid = &group.gid;
  if (snd_pcm_plugin_setup(pcm_handle, &setup) < 0)
  {
    audio_error = _("can't set up plugin");
    return -1;
  }

  if (snd_mixer_open(&mixer_handle, 0, setup.mixer_device) < 0)
  {
    audio_error = _("can't open mixer");
    return -1;
  }

  return 0;
}

static
int play(struct audio_play *play)
{
  unsigned char data[MAX_NSAMPLES * 2 * 2];
  int written = 0;
  int total_bytes;

  audio_pcm_s16le(data, play->nsamples,
		  play->samples[0], play->samples[1], play->mode, play->stats);

  total_bytes = (play->samples[1] ? 4 : 2) * play->nsamples;

  while ((written += snd_pcm_plugin_write(pcm_handle, data + written,
                                          total_bytes - written)) < total_bytes)
  {
    snd_pcm_channel_status_t status;

    memset(&status, 0, sizeof(status));
    if (snd_pcm_plugin_status(pcm_handle, &status) < 0)
    {
      audio_error = _("can't get status to recover playback");
      return -1;
    }

    if (status.status == SND_PCM_STATUS_READY ||
        status.status == SND_PCM_STATUS_UNDERRUN)
    {
      if (snd_pcm_plugin_prepare(pcm_handle, SND_PCM_CHANNEL_PLAYBACK) < 0)
      {
        audio_error = _("can't prepare device to recover playback");
	return -1;
      }
    }

    if (written < 0)
      written = 0;
  }

  return 0;
}

static
int stop(struct audio_stop *stop)
{
  snd_pcm_plugin_playback_drain(pcm_handle);
  return 0;
}

static
int finish(struct audio_finish *finish)
{
  snd_pcm_plugin_flush(pcm_handle, SND_PCM_CHANNEL_PLAYBACK);
  snd_pcm_close(pcm_handle);

  return 0;
}

int audio_qnx(union audio_control *control)
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
