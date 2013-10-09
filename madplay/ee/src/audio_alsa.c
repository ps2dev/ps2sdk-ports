/*
 * madplay - MPEG audio decoder and player
 * Copyright (C) 2000-2004 Robert Leslie
 * ALSA audio output module (C) 2002 Hod McWuff
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
 * $Id: audio_alsa.c,v 1.6 2004/02/23 21:35:23 rob Exp $
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

#include "global.h"

#include <errno.h>

#define ALSA_PCM_OLD_HW_PARAMS_API
#define ALSA_PCM_OLD_SW_PARAMS_API
#include <alsa/asoundlib.h>

#include <mad.h>

#include "audio.h"

char *buf	= NULL;
int paused	= 0;

int rate	= -1;
int channels	= -1;
int bitdepth	= -1;
int sample_size	= -1;

int buffer_time		= 500000;
int period_time		= 100000;
char *defaultdev	= "plughw:0,0";

snd_pcm_hw_params_t *alsa_hwparams;
snd_pcm_sw_params_t *alsa_swparams;

snd_pcm_sframes_t buffer_size;
snd_pcm_sframes_t period_size;

snd_pcm_format_t  alsa_format = -1;
snd_pcm_access_t  alsa_access = SND_PCM_ACCESS_MMAP_INTERLEAVED;

snd_pcm_t *alsa_handle		= NULL;

static audio_pcmfunc_t *audio_pcm;

static
int set_hwparams(snd_pcm_t *handle,
		 snd_pcm_hw_params_t *params,
		 snd_pcm_access_t access)
{
	int err, dir;
	
	/* choose all parameters */
	err = snd_pcm_hw_params_any(handle,params);
	if (err < 0) {
		printf("Access type not available for playback: %s\n", snd_strerror(err));
		return err;
	}
	/* set the sample format */
	err = snd_pcm_hw_params_set_format(handle, params, alsa_format);
	if (err < 0) {
		printf("Sample format not available for playback: %s\n", snd_strerror(err));
		return err;
	}
	/* set the count of channels */
	err = snd_pcm_hw_params_set_channels(handle, params, channels);
	if (err < 0) {
		printf("Channels count (%i) not available for playbacks: %s\n", channels, snd_strerror(err));
		return err;
	}
	/* set the stream rate */
	err = snd_pcm_hw_params_set_rate_near(handle, params, rate, 0);
	if (err < 0) {
		printf("Rate %iHz not available for playback: %s\n", rate, snd_strerror(err));
		return err;
	}
	if (err != rate) {
		printf("Rate doesn't match (requested %iHz, get %iHz)\n", rate, err);
		return -EINVAL;
	}
	/* set buffer time */
	err = snd_pcm_hw_params_set_buffer_time_near(handle, params, buffer_time, &dir);
	if (err < 0) {
		printf("Unable to set buffer time %i for playback: %s\n", buffer_time, snd_strerror(err));
		return err;
	}
	buffer_size = snd_pcm_hw_params_get_buffer_size(params);
	/* set period time */
	err = snd_pcm_hw_params_set_period_time_near(handle, params, period_time, &dir);
	if (err < 0) {
		printf("Unable to set period time %i for playback: %s\n", period_time, snd_strerror(err));
		return err;
	}
	period_size = snd_pcm_hw_params_get_period_size(params, &dir);
	/* write the parameters to device */
	err = snd_pcm_hw_params(handle, params);
	if (err < 0) {
		printf("Unable to set hw params for playback: %s\n", snd_strerror(err));
		return err;
	}
	return 0;
}

static
int set_swparams(snd_pcm_t *handle,
		 snd_pcm_sw_params_t *params)
{
	int err;

        /* get current swparams */
        err = snd_pcm_sw_params_current(handle, params);
        if (err < 0) {
                printf("Unable to determine current swparams for playback: %s\n", snd_strerror(err));
                return err;
        }
        /* start transfer when the buffer is full */
        err = snd_pcm_sw_params_set_start_threshold(handle, params, buffer_size);
        if (err < 0) {
                printf("Unable to set start threshold mode for playback: %s\n", snd_strerror(err));
                return err;
										        }
        /* allow transfer when at least period_size samples can be processed */
        err = snd_pcm_sw_params_set_avail_min(handle, params, period_size);
        if (err < 0) {
                printf("Unable to set avail min for playback: %s\n", snd_strerror(err));
                return err;
												        }
        /* align all transfers to 1 samples */
        err = snd_pcm_sw_params_set_xfer_align(handle, params, 1);
        if (err < 0) {
                printf("Unable to set transfer align for playback: %s\n", snd_strerror(err));
                return err;
        }
        /* write the parameters to device */
        err = snd_pcm_sw_params(handle, params);
        if (err < 0) {
                printf("Unable to set sw params for playback: %s\n", snd_strerror(err));
                return err;
        }
        return 0;
}


static
int init(struct audio_init *init)
{
	int err;

	if (init->path)
		err = snd_pcm_open(&alsa_handle, init->path, SND_PCM_STREAM_PLAYBACK, 0);
	else 
		err = snd_pcm_open(&alsa_handle, defaultdev, SND_PCM_STREAM_PLAYBACK, 0);

	if (err < 0) {
		audio_error=snd_strerror(err);
		return -1;
	}

	return 0;
}

static
int config(struct audio_config *config)
{
	int err;

	snd_pcm_hw_params_alloca(&alsa_hwparams);
	snd_pcm_sw_params_alloca(&alsa_swparams);

	bitdepth	= config->precision;
	channels	= config->channels;
	rate		= config->speed;

	if ( bitdepth == 0 )
		config->precision = bitdepth = 32;

	switch (bitdepth)
	{
		case 8:
			alsa_format = SND_PCM_FORMAT_U8;
			audio_pcm   = audio_pcm_u8;
			break;
		case 16:
			alsa_format = SND_PCM_FORMAT_S16;
#if __BYTE_ORDER == __LITTLE_ENDIAN
			audio_pcm = audio_pcm_s16le;
#else
			audio_pcm = audio_pcm_s16be;
#endif
			break;
		case 24:
			config->precision = bitdepth = 32;
		case 32:
			alsa_format = SND_PCM_FORMAT_S32;
#if __BYTE_ORDER == __LITTLE_ENDIAN
			audio_pcm = audio_pcm_s32le;
#else
			audio_pcm = audio_pcm_s32be;
#endif
			break;
		default:
			audio_error="bitdepth not one of [8,16,24,32]";
			return -1;
	}

	sample_size	= bitdepth * channels / 8;

	err = set_hwparams(alsa_handle, alsa_hwparams, SND_PCM_ACCESS_MMAP_NONINTERLEAVED);
	if (err < 0) {
		audio_error=snd_strerror(err);
		return -1;
	}

	err = set_swparams(alsa_handle, alsa_swparams);
	if (err < 0) {
		audio_error=snd_strerror(err);
		return -1;
	}

	err = snd_pcm_prepare(alsa_handle);
	if (err < 0) {
		audio_error=snd_strerror(err);
		return -1;
	}

	buf = malloc(buffer_size);
	if (buf == NULL) {
		audio_error="unable to allocate output buffer table";
		return -1;
	}

	return 0;

}

static
int xrun_recovery(snd_pcm_t *handle, int err)
{
	if (err == -EPIPE) {		/* underrun */
		err = snd_pcm_prepare(handle);
		if (err < 0) {
			audio_error=snd_strerror(err);
			return -1;
		}
	} else if (err == -ESTRPIPE) {
		while ((err = snd_pcm_resume(handle)) == -EAGAIN)
			sleep(1);	/* wait until suspend flag is gone */
		if (err < 0) {
			err = snd_pcm_prepare(handle);
			if (err < 0) {
				audio_error = snd_strerror(err);
				return -1;
			}
		}
		return 0;
	}
	return err;
}

static
int play(struct audio_play *play)
{
	int err, len;
	char *ptr;

	ptr = buf;
	len = play->nsamples;

	audio_pcm(ptr, len, play->samples[0], play->samples[1],
			play->mode, play->stats);

	while (len > 0) {

		err = snd_pcm_mmap_writei(alsa_handle, ptr, len);

		if (err == -EAGAIN)
			continue;

		if (err < 0) {
			if (xrun_recovery(alsa_handle, err) < 0) {
				audio_error = snd_strerror(err);
				return -1;

			}
			break;
		}

		len -= err;
		ptr += err * sample_size;

	}

	return 0;

}

static
int stop(struct audio_stop *stop)
{
	int err;

	err = snd_pcm_drop(alsa_handle);
	if (err < 0) {
		audio_error = snd_strerror(err);
		return -1;
	}

	err = snd_pcm_prepare(alsa_handle);
	if (err < 0) {
		audio_error = snd_strerror(err);
		return -1;
	}

	return 0;

}

static
int finish(struct audio_finish *finish)
{
	int err;

	err = snd_pcm_close(alsa_handle);
	if (err < 0) {
		audio_error = snd_strerror(err);
		return -1;
	}

	return 0;

}

int audio_alsa(union audio_control *control)
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
