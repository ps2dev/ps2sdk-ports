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

# include <Carbon/Carbon.h>
# include <mad.h>

# include "gettext.h"

# include "audio.h"

static SndChannelPtr channel;
static audio_pcmfunc_t *audio_pcm;

static unsigned int config_channels;
static unsigned int config_speed;
static unsigned int config_precision;

# define NBUFFERS	16
# define NQUEUESAMPLES	1152

static struct buffer {
  ExtSoundHeader header;
  MPSemaphoreID semaphore;
  unsigned int pcm_nsamples;
  unsigned int pcm_length;
  unsigned char pcm_data[MAX_NSAMPLES * 2 * 2];
} output[NBUFFERS];

static int bindex;

enum mode {
  QUEUE,
  IMMEDIATE
};

static
int soundcmd(enum mode mode, unsigned short cmd, short param1, long param2)
{
  SndCommand command;

  command.cmd    = cmd;
  command.param1 = param1;
  command.param2 = param2;

  switch (mode) {
  case IMMEDIATE:
    if (SndDoImmediate(channel, &command) != noErr) {
      audio_error = _("SndDoImmediate() failed");
      return -1;
    }
    break;

  case QUEUE:
    if (SndDoCommand(channel, &command, FALSE) != noErr) {
      audio_error = _("SndDoCommand() failed");
      return -1;
    }
    break;
  }

  return 0;
}

static
void callback(SndChannelPtr channel, SndCommand *command)
{
  struct buffer *buffer = (struct buffer *) command->param2;

  MPSignalSemaphore(buffer->semaphore);
}

static
int wait(struct buffer *buffer)
{
  if (MPWaitOnSemaphore(buffer->semaphore, kDurationForever) != noErr) {
    audio_error = _("MPWaitOnSemaphore() failed");
    return -1;
  }

  return 0;
}

static
int init(struct audio_init *init)
{
  int i;

  channel = 0;

  if (SndNewChannel(&channel, sampledSynth, 0, callback) != noErr) {
    audio_error = _("SndNewChannel() failed");
    return -1;
  }

  for (i = 0; i < NBUFFERS; ++i) {
    if (MPCreateBinarySemaphore(&output[i].semaphore) != noErr) {
      while (i--)
	MPDeleteSemaphore(output[i].semaphore);

      audio_error = _("failed to create synchronization object");
      return -1;
    }

    output[i].pcm_nsamples = 0;
    output[i].pcm_length   = 0;
  }

  bindex = 0;

  return 0;
}

static
int set_pause(int flag)
{
  static int paused;

  if (flag != paused) {
    paused = 0;

    if (flag) {
      if (soundcmd(IMMEDIATE, pauseCmd, 0, 0) == -1 ||
	  soundcmd(IMMEDIATE, quietCmd, 0, 0) == -1)
	return -1;
    }
    else if (soundcmd(IMMEDIATE, resumeCmd, 0, 0) == -1)
      return -1;

    paused = flag;
  }

  return 0;
}

static
void init_header(struct ExtSoundHeader *header, Ptr samples,
		 unsigned int nsamples, unsigned int channels,
		 unsigned int speed, unsigned int bits)
{
  double dspeed = speed;

  header->samplePtr        = samples;
  header->numChannels      = channels;
  header->sampleRate       = FixRatio(speed, 1);
  header->loopStart        = 0;
  header->loopEnd          = 0;
  header->encode           = extSH;
  header->baseFrequency    = kMiddleC;
  header->numFrames        = nsamples;
  dtox80(&dspeed, &header->AIFFSampleRate);
  header->markerChunk      = 0;
  header->instrumentChunks = 0;
  header->AESRecording     = 0;
  header->sampleSize       = bits;
  header->futureUse1       = 0;
  header->futureUse2       = 0;
  header->futureUse3       = 0;
  header->futureUse4       = 0;
}

static
int queue(struct buffer *buffer)
{
  init_header(&buffer->header, buffer->pcm_data, buffer->pcm_nsamples,
	      config_channels, config_speed, config_precision);

  if (soundcmd(QUEUE, bufferCmd,   0, (long) &buffer->header) == -1 ||
      soundcmd(QUEUE, callBackCmd, 0, (long) buffer) == -1)
    return -1;

  return 0;
}

static
int drain(void)
{
  int result = 0;

  if (output[bindex].pcm_nsamples) {
    if (queue(&output[bindex]) == -1)
      result = -1;

    bindex = (bindex + 1) % NBUFFERS;
    output[bindex].pcm_nsamples = 0;
  }

  return result;
}

static
int config(struct audio_config *config)
{
  unsigned int bitdepth;

  if (set_pause(0) == -1)
    return -1;

  bitdepth = config->precision & ~7;
  if (bitdepth == 0 || bitdepth > 16)
    bitdepth = 16;

  if (drain() == -1 ||
      soundcmd(QUEUE, reInitCmd, 0, initNoDrop |
	       (config->channels == 1 ? initMono : initStereo)) == -1)
    return -1;

  switch (config->precision = bitdepth) {
  case 8:
    audio_pcm = audio_pcm_u8;
    break;

  case 16:
    audio_pcm = audio_pcm_s16be;
    break;
  }

  config_channels  = config->channels;
  config_speed     = config->speed;
  config_precision = config->precision;

  return 0;
}

static
int play(struct audio_play *play)
{
  struct buffer *buffer;
  unsigned int len;

  if (set_pause(0) == -1)
    return -1;

  if (output[bindex].pcm_nsamples + play->nsamples > MAX_NSAMPLES &&
      drain() == -1)
    return -1;

  buffer = &output[bindex];

  /* wait for block to finish playing */

  if (buffer->pcm_nsamples == 0) {
    if (wait(buffer) == -1)
      return -1;

    buffer->pcm_length = 0;
  }

  /* prepare block */

  len = audio_pcm(&buffer->pcm_data[buffer->pcm_length], play->nsamples,
		  play->samples[0], play->samples[1], play->mode, play->stats);

  buffer->pcm_nsamples += play->nsamples;
  buffer->pcm_length   += len;

  if (buffer->pcm_nsamples >= NQUEUESAMPLES &&
      drain() == -1)
    return -1;

  return 0;
}

static
int flush(void)
{
  int i, result = 0;

  if (soundcmd(IMMEDIATE, flushCmd, 0, 0) == -1)
    result = -1;

  for (i = 0; i < NBUFFERS; ++i)
    MPSignalSemaphore(output[i].semaphore);

  output[bindex].pcm_nsamples = 0;

  return result;
}

static
int stop(struct audio_stop *stop)
{
  int result;

  result = set_pause(1);

  if (result == 0 && stop->flush && flush() == -1)
    result = -1;

  return result;
}

static
int finish(struct audio_finish *finish)
{
  int i, result = 0;

  if (set_pause(0) == -1 || drain() == -1)
    result = -1;

  if (SndDisposeChannel(channel, FALSE) != noErr && result == 0) {
    audio_error = _("SndDisposeChannel() failed");
    result = -1;
  }

  for (i = 0; i < NBUFFERS; ++i) {
    if (MPDeleteSemaphore(output[i].semaphore) != noErr && result == 0) {
      audio_error = _("failed to delete synchronization object");
      result = -1;
    }
  }

  return result;
}

int audio_carbon(union audio_control *control)
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
