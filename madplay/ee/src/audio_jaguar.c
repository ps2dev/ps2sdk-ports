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

/* N.B. this audio module is unfinished */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "global.h"

# include <CoreServices/CoreServices.h>
# include <AudioUnit/AudioUnit.h>
# include <mad.h>

# include "gettext.h"

# include "audio.h"

static AudioUnit au;
static int started;

# define NBUFFERS  2

struct buffer {
  MPSemaphoreID semaphore;
  unsigned int pcm_nsamples;
  unsigned int pcm_length;
  Float32 pcm_data[48000 * 2];
} output[NBUFFERS];

static int bindex;

static
int start_stop(int start)
{
  if (start && !started) {
    if (AudioOutputUnitStart(au) != 0) {
      audio_error = _("AudioOutputUnitStart() failed");
      return -1;
    }

    started = 1;
  }
  else if (!start && started) {
    if (AudioOutputUnitStop(au) != 0) {
      audio_error = _("AudioOutputUnitStop() failed");
      return -1;
    }

    started = 0;
  }

  return 0;
}

static
OSStatus render(void *inRefCon,
		AudioUnitRenderActionFlags *ioActionFlags,
		const AudioTimeStamp *inTimeStamp,
		UInt32 inBusNumber,
		UInt32 inNumberFrames,
		AudioBufferList *ioData)
{
  AudioBuffer *buffer;
  Float32 *samples, *end;

  printf("ioActionFlags = %lu, inBusNumber = %lu, inNumberFrames = %lu\n",
	 *ioActionFlags, inBusNumber, inNumberFrames);
  printf("mNumberBuffers = %lu, mNumberChannels = %lu, mDataByteSize = %lu\n",
	 ioData->mNumberBuffers, ioData->mBuffers[0].mNumberChannels,
	 ioData->mBuffers[0].mDataByteSize);

  if (ioData->mNumberBuffers != 1)
    return 1000;

  buffer = &ioData->mBuffers[0];

  if (buffer->mNumberChannels != 2)
    return 1001;

  samples = buffer->mData;
  end     = samples + (buffer->mDataByteSize / sizeof(Float32));

  while (samples < end)
    *samples++ = 0;

  *ioActionFlags |= kAudioUnitRenderAction_OutputIsSilence;

  return noErr;
}

static
int init(struct audio_init *init)
{
  ComponentDescription desc;
  Component comp;
  AURenderCallbackStruct callback;
  int i;

  desc.componentType         = kAudioUnitType_Output;
  desc.componentSubType      = kAudioUnitSubType_DefaultOutput;
  desc.componentManufacturer = kAudioUnitManufacturer_Apple;
  desc.componentFlags        = 0;
  desc.componentFlagsMask    = 0;

  comp = FindNextComponent(0, &desc);
  if (comp == 0) {
    audio_error = _("FindNextComponent() failed");
    return -1;
  }

  if (OpenAComponent(comp, &au) != noErr) {
    audio_error = _("OpenAComponent() failed");
    return -1;
  }

  if (AudioUnitInitialize(au) != 0) {
    audio_error = _("AudioUnitInitialize() failed");

    CloseComponent(au);

    return -1;
  }

  callback.inputProc       = render;
  callback.inputProcRefCon = 0;

  if (AudioUnitSetProperty(au, kAudioUnitProperty_SetRenderCallback,
			   kAudioUnitScope_Input, 0,
			   &callback, sizeof(callback)) != 0) {
    audio_error =
      _("AudioUnitSetProperty(kAudioUnitProperty_SetRenderCallback) failed");

    AudioUnitUninitialize(au);
    CloseComponent(au);

    return -1;
  }

  for (i = 0; i < NBUFFERS; ++i) {
    if (MPCreateBinarySemaphore(&output[i].semaphore) != noErr) {
      audio_error = _("failed to create synchronization object");

      while (i--)
	MPDeleteSemaphore(output[i].semaphore);

      AudioUnitUninitialize(au);
      CloseComponent(au);

      return -1;
    }

    output[i].pcm_nsamples = 0;
    output[i].pcm_length   = 0;
  }

  started = 0;
  bindex  = 0;

  return 0;
}

static
void set_format(AudioStreamBasicDescription *format,
		unsigned int channels, Float64 speed)
{
  format->mSampleRate       = speed;
  format->mFormatID         = kAudioFormatLinearPCM;
  format->mFormatFlags      = kLinearPCMFormatFlagIsFloat |
                              kLinearPCMFormatFlagIsBigEndian |
                              kLinearPCMFormatFlagIsPacked;
  format->mBytesPerPacket   = channels * sizeof(Float32);
  format->mFramesPerPacket  = 1;
  format->mBytesPerFrame    = channels * sizeof(Float32);
  format->mChannelsPerFrame = channels;
  format->mBitsPerChannel   = 8 * sizeof(Float32);
}

static
int config(struct audio_config *config)
{
  AudioStreamBasicDescription format;

  /* synchronize somehow with running AudioUnit... */

  /* config->channels  = 2; */
  config->precision = 24;

  set_format(&format, config->channels, config->speed);

  if (AudioUnitSetProperty(au, kAudioUnitProperty_StreamFormat,
			   kAudioUnitScope_Input, 0,
			   &format, sizeof(format)) != 0) {
    audio_error =
      _("AudioUnitSetProperty(kAudioUnitProperty_StreamFormat) failed");
    return -1;
  }

  return 0;
}

/*
 * NAME:	float32()
 * DESCRIPTION:	store 32-bit IEEE Standard 754 floating point representation
 */
static
void float32(void **ptr, mad_fixed_t sample)
{
  unsigned long **mem = (void *) ptr, ieee = 0;

  if (sample) {
    unsigned int zc;
    unsigned long abs, s, e, f;

    enum {
      BIAS = 0x7f
    };

    /* |seeeeeee|efffffff|ffffffff|ffffffff| */

    s = sample & 0x80000000UL;
    abs = s ? -sample : sample;

    /* PPC count leading zeros */
    asm ("cntlzw %0,%1" : "=r" (zc) : "r" (abs));

    /* calculate exponent */

    e = (((32 - MAD_F_FRACBITS - 1) - zc + BIAS) << 23) & 0x7f800000UL;

    /* normalize 1.f - round? */

    f = ((abs << zc) >> 8) & 0x007fffffUL;

    ieee = s | e | f;
  }

  *(*mem)++ = ieee;
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
int wait(struct buffer *buffer)
{
  if (MPWaitOnSemaphore(buffer->semaphore, kDurationForever) != noErr) {
    audio_error = _("MPWaitOnSemaphore() failed");
    return -1;
  }

  return 0;
}

static
int play(struct audio_play *play)
{
  struct buffer *buffer;

  update_stats(play->stats, play->nsamples, play->samples[0]);
  if (play->samples[1])
    update_stats(play->stats, play->nsamples, play->samples[1]);

  buffer = &output[bindex];

  /* wait for block to finish playing */

  if (buffer->pcm_nsamples == 0) {
    if (wait(buffer) == -1)
      return -1;

    buffer->pcm_length = 0;
  }

  start_stop(1);

  return 0;
}

static
int stop(struct audio_stop *stop)
{
  start_stop(0);

  return 0;
}

static
int finish(struct audio_finish *finish)
{
  int i, result = 0;

  start_stop(0);

  if (AudioUnitUninitialize(au) != 0 && result == 0) {
    audio_error = _("AudioUnitUninitialize() failed");
    result = -1;
  }

  if (CloseComponent(au) != noErr && result == 0) {
    audio_error = _("CloseComponent() failed");
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

int audio_jaguar(union audio_control *control)
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
