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

static FILE *outfile;
static audio_pcmfunc_t *audio_pcm;

static unsigned long riff_len, data_len;
static long data_chunk;

static unsigned int config_channels;
static unsigned int config_speed;
static unsigned int config_precision;

# define WAVE_FORMAT_PCM	0x0001
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

  /* RIFF header and (WAVE) data type identifier */

  if (fwrite("RIFF" UNKNOWN_LENGTH "WAVE", 4 + 4 + 4, 1, outfile) != 1) {
    audio_error = ":fwrite";
    return -1;
  }

  riff_len   =  4;
  data_chunk = -1;

  config_channels  = 0;
  config_speed     = 0;
  config_precision = 0;

  return 0;
}

/*
 * NAME:	int32()
 * DESCRIPTION:	store 32-bit little-endian integer
 */
static
void int32(unsigned char *ptr, unsigned long num)
{
  ptr[0] = num >>  0;
  ptr[1] = num >>  8;
  ptr[2] = num >> 16;
  ptr[3] = num >> 24;
}

/*
 * NAME:	int16()
 * DESCRIPTION:	store 16-bit little-endian integer
 */
static
void int16(unsigned char *ptr, unsigned int num)
{
  ptr[0] = num >> 0;
  ptr[1] = num >> 8;
}

static
int config(struct audio_config *config)
{
  unsigned char chunk[8 + 16];
  unsigned int block_al, bitdepth;
  unsigned long bytes_ps;

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

  /* Format chunk */

  block_al = config->channels * ((bitdepth + 7) / 8);
  bytes_ps = config->speed * block_al;

  memcpy(&chunk[0], "fmt ", 4);		/* chunkID */
  int32(&chunk[4],  16);		/* chunkSize */

  int16(&chunk[8],  WAVE_FORMAT_PCM);	/* wFormatTag */
  int16(&chunk[10], config->channels);	/* wChannels */
  int32(&chunk[12], config->speed);	/* dwSamplesPerSec */
  int32(&chunk[16], bytes_ps);		/* dwAvgBytesPerSec */
  int16(&chunk[20], block_al);		/* wBlockAlign */

  /* PCM-format-specific */

  int16(&chunk[22], bitdepth);		/* wBitsPerSample */

  if (fwrite(chunk, 8 + 16, 1, outfile) != 1) {
    audio_error = ":fwrite";
    return -1;
  }

  riff_len += 8 + 16;

  /* Data chunk */

  data_chunk = ftell(outfile);

  if (fwrite("data" UNKNOWN_LENGTH, 8 + 0, 1, outfile) != 1) {
    audio_error = ":fwrite";
    return -1;
  }

  riff_len += 8 + 0;
  data_len  = 0;

  switch (config->precision = bitdepth) {
  case 1: case 2: case 3: case 4:
  case 5: case 6: case 7: case 8:
    audio_pcm = audio_pcm_u8;
    break;

  case  9: case 10: case 11: case 12:
  case 13: case 14: case 15: case 16:
    audio_pcm = audio_pcm_s16le;
    break;

  case 17: case 18: case 19: case 20:
  case 21: case 22: case 23: case 24:
    audio_pcm = audio_pcm_s24le;
    break;

  case 25: case 26: case 27: case 28:
  case 29: case 30: case 31: case 32:
    audio_pcm = audio_pcm_s32le;
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
  unsigned char data[MAX_NSAMPLES * 4 * 2];
  unsigned int len;

  len = audio_pcm(data, play->nsamples, play->samples[0], play->samples[1],
		  play->mode, play->stats);

  if (fwrite(data, len, 1, outfile) != 1) {
    audio_error = ":fwrite";
    return -1;
  }

  data_len += len;
  riff_len += len;

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

  if (data_len & 1) {
    if (fputc(0, outfile) == EOF && result == 0) {
      audio_error = ":fputc";
      result = -1;
    }

    ++riff_len;
  }

  if (result == 0 &&
      data_chunk != -1 && patch_int32(data_chunk + 4, data_len) == -1)
    result = -1;

  if (result == 0)
    patch_int32(4, riff_len);

  if (outfile != stdout &&
      fclose(outfile) == EOF &&
      result == 0) {
    audio_error = ":fclose";
    result = -1;
  }

  return result;
}

int audio_wave(union audio_control *control)
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
