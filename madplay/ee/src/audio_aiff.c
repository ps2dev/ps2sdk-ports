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
 * $Id: audio_aiff.c,v 1.7 2004/01/23 09:41:31 rob Exp $
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
static audio_pcmfunc_t *audio_pcm;
static unsigned long nsamples;

static unsigned long form_len, ssnd_len;
static long comm_chunk, ssnd_chunk;

static unsigned int config_channels;
static unsigned int config_speed;
static unsigned int config_precision;

# define UNKNOWN_LENGTH		"\xff\xff\xff\xff"

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

  /* group ID and file type ID header */

  if (fwrite("FORM" UNKNOWN_LENGTH "AIFF", 4 + 4 + 4, 1, outfile) != 1) {
    audio_error = ":fwrite";
    return -1;
  }

  form_len   =  4;
  comm_chunk = -1;
  ssnd_chunk = -1;

  config_channels  = 0;
  config_speed     = 0;
  config_precision = 0;

  return 0;
}

/*
 * NAME:	int32()
 * DESCRIPTION:	store 32-bit big-endian integer
 */
static
void int32(unsigned char *ptr, unsigned long num)
{
  ptr[0] = num >> 24;
  ptr[1] = num >> 16;
  ptr[2] = num >>  8;
  ptr[3] = num >>  0;
}

/*
 * NAME:	int16()
 * DESCRIPTION:	store 16-bit big-endian integer
 */
static
void int16(unsigned char *ptr, unsigned int num)
{
  ptr[0] = num >> 8;
  ptr[1] = num >> 0;
}

/*
 * NAME:	float80()
 * DESCRIPTION:	store 80-bit IEEE Standard 754 floating point number
 */
static
void float80(unsigned char *ptr, signed long num)
{
  if (num == 0)
    memset(ptr, 0, 10);
  else {
    int exp;
    unsigned int s, e;
    unsigned long abs, sr, f;

    enum {
      BIAS = 0x3fff
    };

    s = num < 0;
    abs = s ? -num : num;

    for (exp = -1, sr = abs; sr; sr >>= 1)
      ++exp;

    e = exp + BIAS;

    /* write normalized value; high bit of f (i) is always set */

    f = abs << (32 - exp - 1);

    ptr[0] = (s <<  7) | (e >> 8);
    ptr[1] = (e >>  0);
    ptr[2] = (f >> 24);
    ptr[3] = (f >> 16);
    ptr[4] = (f >>  8);
    ptr[5] = (f >>  0);
    ptr[6] = 0;
    ptr[7] = 0;
    ptr[8] = 0;
    ptr[9] = 0;
  }
}

static
int config(struct audio_config *config)
{
  unsigned int bitdepth;
  unsigned char chunk[8 + 18];

  if (config_precision) {
    /* it's not possible to change the format once set */

    config->channels  = config_channels;
    config->speed     = config_speed;
    config->precision = config_precision;

    return 0;
  }

  bitdepth = config->precision;
  if (bitdepth == 0)
    bitdepth = 16;
  else if (bitdepth > 32)
    bitdepth = 32;

  /* Common chunk */

  comm_chunk = ftell(outfile);

  memcpy(&chunk[0],   "COMM", 4);		/* chunkID */
  int32(&chunk[4],    18);			/* chunkSize */

  int16(&chunk[8],    config->channels);	/* numChannels */
  int32(&chunk[10],   ~0L);			/* numSampleFrames */
  int16(&chunk[14],   bitdepth);		/* sampleSize */
  float80(&chunk[16], config->speed);		/* sampleRate */

  if (fwrite(chunk, 8 + 18, 1, outfile) != 1) {
    audio_error = ":fwrite";
    return -1;
  }

  form_len += 8 + 18;

  /* Sound Data chunk */

  ssnd_chunk = ftell(outfile);

  memcpy(&chunk[0], "SSND", 4);		/* chunkID */
  int32(&chunk[4],  ~0L);		/* chunkSize */

  int32(&chunk[8],  0);			/* offset */
  int32(&chunk[12], 0);			/* blockSize */

  if (fwrite(chunk, 8 + 8, 1, outfile) != 1) {
    audio_error = ":fwrite";
    return -1;
  }

  form_len += 8 + 8;
  ssnd_len  = 8;

  switch (config->precision = bitdepth) {
  case 1: case 2: case 3: case 4:
  case 5: case 6: case 7: case 8:
    audio_pcm = audio_pcm_s8;
    break;

  case  9: case 10: case 11: case 12:
  case 13: case 14: case 15: case 16:
    audio_pcm = audio_pcm_s16be;
    break;

  case 17: case 18: case 19: case 20:
  case 21: case 22: case 23: case 24:
    audio_pcm = audio_pcm_s24be;
    break;

  case 25: case 26: case 27: case 28:
  case 29: case 30: case 31: case 32:
    audio_pcm = audio_pcm_s32be;
    break;
  }

  nsamples = 0;

  config_channels  = config->channels;
  config_speed     = config->speed;
  config_precision = config->precision;

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

  nsamples += play->nsamples;

  ssnd_len += len;
  form_len += len;

  return 0;
}

static
int stop(struct audio_stop *stop)
{
  return 0;
}

static
int patch_int32(long address, unsigned long num)
{
  unsigned char dword[4];

  if (fseek(outfile, address, SEEK_SET) == -1) {
    audio_error = ":fseek";
    return -1;
  }

  int32(dword, num);

  if (fwrite(dword, sizeof(dword), 1, outfile) != 1) {
    audio_error = ":fwrite";
    return -1;
  }

  if (fseek(outfile, 0, SEEK_END) == -1) {
    audio_error = ":fseek";
    return -1;
  }

  return 0;
}

static
int finish(struct audio_finish *finish)
{
  int result = 0;

  if (config_precision == 0) {
    struct audio_config dummy;

    /* write empty chunks */

    dummy.command   = AUDIO_COMMAND_CONFIG;
    dummy.channels  = 2;
    dummy.speed     = 44100;
    dummy.precision = 0;

    result = config(&dummy);
  }

  if (ssnd_len & 1) {
    if (fputc(0, outfile) == EOF && result == 0) {
      audio_error = ":fputc";
      result = -1;
    }

    ++form_len;
  }

  if (result == 0 &&
      ((comm_chunk != -1 && patch_int32(comm_chunk + 10, nsamples) == -1) ||
       (ssnd_chunk != -1 && patch_int32(ssnd_chunk +  4, ssnd_len) == -1)))
    result = -1;

  if (result == 0)
    patch_int32(4, form_len);

  if (outfile != stdout &&
      fclose(outfile) == EOF &&
      result == 0) {
    audio_error = ":fclose";
    result = -1;
  }

  return result;
}

int audio_aiff(union audio_control *control)
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
