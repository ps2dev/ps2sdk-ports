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

# ifndef AUDIO_H
# define AUDIO_H

# include <mad.h>

# define MAX_RESAMPLEFACTOR	6
# define MAX_NSAMPLES		(1152 * MAX_RESAMPLEFACTOR)

enum audio_command {
  AUDIO_COMMAND_INIT,
  AUDIO_COMMAND_CONFIG,
  AUDIO_COMMAND_PLAY,
  AUDIO_COMMAND_STOP,
  AUDIO_COMMAND_FINISH
};

enum audio_mode {
  AUDIO_MODE_ROUND,
  AUDIO_MODE_DITHER
};

struct audio_stats {
  unsigned long clipped_samples;
  mad_fixed_t peak_clipping;
  mad_fixed_t peak_sample;
};

union audio_control {
  enum audio_command command;

  struct audio_init {
    enum audio_command command;
    char const *path;
  } init;

  struct audio_config {
    enum audio_command command;
    unsigned int channels;
    unsigned int speed;
    unsigned int precision;
  } config;

  struct audio_play {
    enum audio_command command;
    unsigned int nsamples;
    mad_fixed_t const *samples[2];
    enum audio_mode mode;
    struct audio_stats *stats;
  } play;

  struct audio_stop {
    enum audio_command command;
    int flush;
  } stop;

  struct audio_finish {
    enum audio_command command;
  } finish;
};

struct audio_dither {
  mad_fixed_t error[3];
  mad_fixed_t random;
};

extern char const *audio_error;

typedef int audio_ctlfunc_t(union audio_control *);

audio_ctlfunc_t *audio_output(char const **);

audio_ctlfunc_t audio_alsa;
audio_ctlfunc_t audio_carbon;
audio_ctlfunc_t audio_empeg;
audio_ctlfunc_t audio_esd;
audio_ctlfunc_t audio_sjpcm;
audio_ctlfunc_t audio_jaguar;
audio_ctlfunc_t audio_nas;
audio_ctlfunc_t audio_oss;
audio_ctlfunc_t audio_qnx;
audio_ctlfunc_t audio_sun;
audio_ctlfunc_t audio_win32;

audio_ctlfunc_t audio_aiff;
audio_ctlfunc_t audio_cdda;
audio_ctlfunc_t audio_hex;
audio_ctlfunc_t audio_null;
audio_ctlfunc_t audio_raw;
audio_ctlfunc_t audio_snd;
audio_ctlfunc_t audio_wave;

void audio_control_init(union audio_control *, enum audio_command);

signed long audio_linear_round(unsigned int, mad_fixed_t,
			       struct audio_stats *);
signed long audio_linear_dither(unsigned int, mad_fixed_t,
				struct audio_dither *, struct audio_stats *);

unsigned char audio_mulaw_round(mad_fixed_t, struct audio_stats *);
unsigned char audio_mulaw_dither(mad_fixed_t, struct audio_dither *,
				 struct audio_stats *);

typedef unsigned int audio_pcmfunc_t(unsigned char *, unsigned int,
				     mad_fixed_t const *, mad_fixed_t const *,
				     enum audio_mode, struct audio_stats *);

audio_pcmfunc_t audio_pcm_u8;

audio_pcmfunc_t audio_pcm_s8;
audio_pcmfunc_t audio_pcm_s16le;
audio_pcmfunc_t audio_pcm_s16be;
audio_pcmfunc_t audio_pcm_s24le;
audio_pcmfunc_t audio_pcm_s24be;
audio_pcmfunc_t audio_pcm_s32le;
audio_pcmfunc_t audio_pcm_s32be;

audio_pcmfunc_t audio_pcm_mulaw;

# endif
