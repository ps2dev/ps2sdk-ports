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

# include <string.h>
# include <mad.h>

# include "audio.h"

char const *audio_error;

static struct audio_dither left_dither, right_dither;

# if defined(_MSC_VER)
#  pragma warning(disable: 4550)  /* expression evaluates to a function which
				     is missing an argument list */
# endif

/*
 * NAME:	audio_output()
 * DESCRIPTION: choose an audio output module from a specifier pathname
 */
audio_ctlfunc_t *audio_output(char const **path)
{
  char const *ext;
  int i;

  struct map {
    char const *name;
    audio_ctlfunc_t *module;
  };

  struct map const prefixes[] = {
    { "cdda", audio_cdda },
    { "aiff", audio_aiff },
    { "wave", audio_wave },
    { "wav",  audio_wave },
    { "snd",  audio_snd  },
    { "au",   audio_snd  },
    { "raw",  audio_raw  },
    { "pcm",  audio_raw  },
    { "hex",  audio_hex  },
    { "sjpcm",  audio_sjpcm  },
# if defined(HAVE_LIBESD)
    { "esd",  audio_esd  },
# endif
# if defined(HAVE_LIBAUDIO)
    { "nas",  audio_nas  },
# endif
    { "null", audio_null },
    { "nul",  audio_null }
  };

  struct map const extensions[] = {
    { "cdr",  audio_cdda },
    { "cda",  audio_cdda },
    { "cdda", audio_cdda },
    { "aif",  audio_aiff },
    { "aiff", audio_aiff },
    { "wav",  audio_wave },
    { "snd",  audio_snd  },
    { "au",   audio_snd  },
    { "raw",  audio_raw  },
    { "pcm",  audio_raw  },
    { "out",  audio_raw  },
    { "bin",  audio_raw  },
    { "hex",  audio_hex  },
    { "txt",  audio_hex  }
  };

  if (path == 0)
    return AUDIO_DEFAULT;

  /* check for prefix specifier */

  ext = strchr(*path, ':');
  if (ext) {
    char const *type;

    type  = *path;
    *path = ext + 1;

    for (i = 0; i < sizeof(prefixes) / sizeof(prefixes[0]); ++i) {
      if (strncasecmp(type, prefixes[i].name, ext - type) == 0 &&
	  strlen(prefixes[i].name) == ext - type)
	return prefixes[i].module;
    }

    *path = type;
    return 0;
  }

  if (strcmp(*path, "/dev/null") == 0)
    return audio_null;

  if (strncmp(*path, "/dev/", 5) == 0)
    return AUDIO_DEFAULT;

  /* check for file extension specifier */

  ext = strrchr(*path, '.');
  if (ext) {
    ++ext;

    for (i = 0; i < sizeof(extensions) / sizeof(extensions[0]); ++i) {
      if (strcasecmp(ext, extensions[i].name) == 0)
	return extensions[i].module;
    }
  }

  return 0;
}

/*
 * NAME:	audio_control_init()
 * DESCRIPTION:	initialize an audio control structure
 */
void audio_control_init(union audio_control *control,
			enum audio_command command)
{
  switch (control->command = command) {
  case AUDIO_COMMAND_INIT:
    control->init.path = 0;
    break;

  case AUDIO_COMMAND_CONFIG:
    control->config.channels  = 0;
    control->config.speed     = 0;
    control->config.precision = 0;
    break;

  case AUDIO_COMMAND_PLAY:
    control->play.nsamples   = 0;
    control->play.samples[0] = 0;
    control->play.samples[1] = 0;
    control->play.mode       = AUDIO_MODE_DITHER;
    control->play.stats      = 0;
    break;

  case AUDIO_COMMAND_STOP:
    control->stop.flush = 0;
    break;

  case AUDIO_COMMAND_FINISH:
    break;
  }
}

/*
 * NAME:	clip()
 * DESCRIPTION:	gather signal statistics while clipping
 */
static inline
void clip(mad_fixed_t *sample, struct audio_stats *stats)
{
  enum {
    MIN = -MAD_F_ONE,
    MAX =  MAD_F_ONE - 1
  };

  if (*sample >= stats->peak_sample) {
    if (*sample > MAX) {
      ++stats->clipped_samples;
      if (*sample - MAX > stats->peak_clipping)
	stats->peak_clipping = *sample - MAX;

      *sample = MAX;
    }
    stats->peak_sample = *sample;
  }
  else if (*sample < -stats->peak_sample) {
    if (*sample < MIN) {
      ++stats->clipped_samples;
      if (MIN - *sample > stats->peak_clipping)
	stats->peak_clipping = MIN - *sample;

      *sample = MIN;
    }
    stats->peak_sample = -*sample;
  }
}

/*
 * NAME:	audio_linear_round()
 * DESCRIPTION:	generic linear sample quantize routine
 */
# if defined(_MSC_VER)
extern  /* needed to satisfy bizarre MSVC++ interaction with inline */
# endif
inline
signed long audio_linear_round(unsigned int bits, mad_fixed_t sample,
			       struct audio_stats *stats)
{
  /* round */
  sample += (1L << (MAD_F_FRACBITS - bits));

  /* clip */
  clip(&sample, stats);

  /* quantize and scale */
  return sample >> (MAD_F_FRACBITS + 1 - bits);
}

/*
 * NAME:	prng()
 * DESCRIPTION:	32-bit pseudo-random number generator
 */
static inline
unsigned long prng(unsigned long state)
{
  return (state * 0x0019660dL + 0x3c6ef35fL) & 0xffffffffL;
}

/*
 * NAME:	audio_linear_dither()
 * DESCRIPTION:	generic linear sample quantize and dither routine
 */
# if defined(_MSC_VER)
extern  /* needed to satisfy bizarre MSVC++ interaction with inline */
# endif
inline
signed long audio_linear_dither(unsigned int bits, mad_fixed_t sample,
				struct audio_dither *dither,
				struct audio_stats *stats)
{
  unsigned int scalebits;
  mad_fixed_t output, mask, random;

  enum {
    MIN = -MAD_F_ONE,
    MAX =  MAD_F_ONE - 1
  };

  /* noise shape */
  sample += dither->error[0] - dither->error[1] + dither->error[2];

  dither->error[2] = dither->error[1];
  dither->error[1] = dither->error[0] / 2;

  /* bias */
  output = sample + (1L << (MAD_F_FRACBITS + 1 - bits - 1));

  scalebits = MAD_F_FRACBITS + 1 - bits;
  mask = (1L << scalebits) - 1;

  /* dither */
  random  = prng(dither->random);
  output += (random & mask) - (dither->random & mask);

  dither->random = random;

  /* clip */
  if (output >= stats->peak_sample) {
    if (output > MAX) {
      ++stats->clipped_samples;
      if (output - MAX > stats->peak_clipping)
	stats->peak_clipping = output - MAX;

      output = MAX;

      if (sample > MAX)
	sample = MAX;
    }
    stats->peak_sample = output;
  }
  else if (output < -stats->peak_sample) {
    if (output < MIN) {
      ++stats->clipped_samples;
      if (MIN - output > stats->peak_clipping)
	stats->peak_clipping = MIN - output;

      output = MIN;

      if (sample < MIN)
	sample = MIN;
    }
    stats->peak_sample = -output;
  }

  /* quantize */
  output &= ~mask;

  /* error feedback */
  dither->error[0] = sample - output;

  /* scale */
  return output >> scalebits;
}

/*
 * NAME:	audio_pcm_u8()
 * DESCRIPTION:	write a block of unsigned 8-bit PCM samples
 */
unsigned int audio_pcm_u8(unsigned char *data, unsigned int nsamples,
			  mad_fixed_t const *left, mad_fixed_t const *right,
			  enum audio_mode mode, struct audio_stats *stats)
{
  unsigned int len;

  len = nsamples;

  if (right) {  /* stereo */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--) {
	data[0] = audio_linear_round(8, *left++,  stats) ^ 0x80;
	data[1] = audio_linear_round(8, *right++, stats) ^ 0x80;

	data += 2;
      }
      break;

    case AUDIO_MODE_DITHER:
      while (len--) {
	data[0] = audio_linear_dither(8, *left++,
				      &left_dither,  stats) ^ 0x80;
	data[1] = audio_linear_dither(8, *right++,
				      &right_dither, stats) ^ 0x80;

	data += 2;
      }
      break;

    default:
      return 0;
    }

    return nsamples * 2;
  }
  else {  /* mono */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--)
	*data++ = audio_linear_round(8, *left++, stats) ^ 0x80;
      break;

    case AUDIO_MODE_DITHER:
      while (len--)
	*data++ = audio_linear_dither(8, *left++, &left_dither, stats) ^ 0x80;
      break;

    default:
      return 0;
    }

    return nsamples;
  }
}

/*
 * NAME:	audio_pcm_s8()
 * DESCRIPTION:	write a block of signed 8-bit PCM samples
 */
unsigned int audio_pcm_s8(unsigned char *data, unsigned int nsamples,
			  mad_fixed_t const *left, mad_fixed_t const *right,
			  enum audio_mode mode, struct audio_stats *stats)
{
  unsigned int len;

  len = nsamples;

  if (right) {  /* stereo */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--) {
	data[0] = audio_linear_round(8, *left++,  stats);
	data[1] = audio_linear_round(8, *right++, stats);

	data += 2;
      }
      break;

    case AUDIO_MODE_DITHER:
      while (len--) {
	data[0] = audio_linear_dither(8, *left++,
				      &left_dither,  stats);
	data[1] = audio_linear_dither(8, *right++,
				      &right_dither, stats);

	data += 2;
      }
      break;

    default:
      return 0;
    }

    return nsamples * 2;
  }
  else {  /* mono */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--)
	*data++ = audio_linear_round(8, *left++, stats);
      break;

    case AUDIO_MODE_DITHER:
      while (len--)
	*data++ = audio_linear_dither(8, *left++, &left_dither, stats);
      break;

    default:
      return 0;
    }

    return nsamples;
  }
}

/*
 * NAME:	audio_pcm_s16le()
 * DESCRIPTION:	write a block of signed 16-bit little-endian PCM samples
 */
unsigned int audio_pcm_s16le(unsigned char *data, unsigned int nsamples,
			     mad_fixed_t const *left, mad_fixed_t const *right,
			     enum audio_mode mode, struct audio_stats *stats)
{
  unsigned int len;
  register signed int sample0, sample1;

  len = nsamples;

  if (right) {  /* stereo */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--) {
	sample0 = audio_linear_round(16, *left++,  stats);
	sample1 = audio_linear_round(16, *right++, stats);

	data[0] = sample0 >> 0;
	data[1] = sample0 >> 8;
	data[2] = sample1 >> 0;
	data[3] = sample1 >> 8;

	data += 4;
      }
      break;

    case AUDIO_MODE_DITHER:
      while (len--) {
	sample0 = audio_linear_dither(16, *left++,  &left_dither,  stats);
	sample1 = audio_linear_dither(16, *right++, &right_dither, stats);

	data[0] = sample0 >> 0;
	data[1] = sample0 >> 8;
	data[2] = sample1 >> 0;
	data[3] = sample1 >> 8;

	data += 4;
      }
      break;

    default:
      return 0;
    }

    return nsamples * 2 * 2;
  }
  else {  /* mono */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--) {
	sample0 = audio_linear_round(16, *left++, stats);

	data[0] = sample0 >> 0;
	data[1] = sample0 >> 8;

	data += 2;
      }
      break;

    case AUDIO_MODE_DITHER:
      while (len--) {
	sample0 = audio_linear_dither(16, *left++, &left_dither, stats);

	data[0] = sample0 >> 0;
	data[1] = sample0 >> 8;

	data += 2;
      }
      break;

    default:
      return 0;
    }

    return nsamples * 2;
  }
}

/*
 * NAME:	audio_pcm_s16be()
 * DESCRIPTION:	write a block of signed 16-bit big-endian PCM samples
 */
unsigned int audio_pcm_s16be(unsigned char *data, unsigned int nsamples,
			     mad_fixed_t const *left, mad_fixed_t const *right,
			     enum audio_mode mode, struct audio_stats *stats)
{
  unsigned int len;
  register signed int sample0, sample1;

  len = nsamples;

  if (right) {  /* stereo */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--) {
	sample0 = audio_linear_round(16, *left++,  stats);
	sample1 = audio_linear_round(16, *right++, stats);

	data[0] = sample0 >> 8;
	data[1] = sample0 >> 0;
	data[2] = sample1 >> 8;
	data[3] = sample1 >> 0;

	data += 4;
      }
      break;

    case AUDIO_MODE_DITHER:
      while (len--) {
	sample0 = audio_linear_dither(16, *left++,  &left_dither,  stats);
	sample1 = audio_linear_dither(16, *right++, &right_dither, stats);

	data[0] = sample0 >> 8;
	data[1] = sample0 >> 0;
	data[2] = sample1 >> 8;
	data[3] = sample1 >> 0;

	data += 4;
      }
      break;

    default:
      return 0;
    }

    return nsamples * 2 * 2;
  }
  else {  /* mono */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--) {
	sample0 = audio_linear_round(16, *left++, stats);

	data[0] = sample0 >> 8;
	data[1] = sample0 >> 0;

	data += 2;
      }
      break;

    case AUDIO_MODE_DITHER:
      while (len--) {
	sample0 = audio_linear_dither(16, *left++, &left_dither, stats);

	data[0] = sample0 >> 8;
	data[1] = sample0 >> 0;

	data += 2;
      }
      break;

    default:
      return 0;
    }

    return nsamples * 2;
  }
}

/*
 * NAME:	audio_pcm_s24le()
 * DESCRIPTION:	write a block of signed 24-bit little-endian PCM samples
 */
unsigned int audio_pcm_s24le(unsigned char *data, unsigned int nsamples,
			     mad_fixed_t const *left, mad_fixed_t const *right,
			     enum audio_mode mode, struct audio_stats *stats)
{
  unsigned int len;
  register signed long sample0, sample1;

  len = nsamples;

  if (right) {  /* stereo */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--) {
	sample0 = audio_linear_round(24, *left++,  stats);
	sample1 = audio_linear_round(24, *right++, stats);

	data[0] = sample0 >>  0;
	data[1] = sample0 >>  8;
	data[2] = sample0 >> 16;

	data[3] = sample1 >>  0;
	data[4] = sample1 >>  8;
	data[5] = sample1 >> 16;

	data += 6;
      }
      break;

    case AUDIO_MODE_DITHER:
      while (len--) {
	sample0 = audio_linear_dither(24, *left++,  &left_dither,  stats);
	sample1 = audio_linear_dither(24, *right++, &right_dither, stats);

	data[0] = sample0 >>  0;
	data[1] = sample0 >>  8;
	data[2] = sample0 >> 16;

	data[3] = sample1 >>  0;
	data[4] = sample1 >>  8;
	data[5] = sample1 >> 16;

	data += 6;
      }
      break;

    default:
      return 0;
    }

    return nsamples * 3 * 2;
  }
  else {  /* mono */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--) {
	sample0 = audio_linear_round(24, *left++, stats);

	data[0] = sample0 >>  0;
	data[1] = sample0 >>  8;
	data[2] = sample0 >> 16;

	data += 3;
      }
      break;

    case AUDIO_MODE_DITHER:
      while (len--) {
	sample0 = audio_linear_dither(24, *left++, &left_dither, stats);

	data[0] = sample0 >>  0;
	data[1] = sample0 >>  8;
	data[2] = sample0 >> 16;

	data += 3;
      }
      break;

    default:
      return 0;
    }

    return nsamples * 3;
  }
}

/*
 * NAME:	audio_pcm_s24be()
 * DESCRIPTION:	write a block of signed 24-bit big-endian PCM samples
 */
unsigned int audio_pcm_s24be(unsigned char *data, unsigned int nsamples,
			     mad_fixed_t const *left, mad_fixed_t const *right,
			     enum audio_mode mode, struct audio_stats *stats)
{
  unsigned int len;
  register signed long sample0, sample1;

  len = nsamples;

  if (right) {  /* stereo */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--) {
	sample0 = audio_linear_round(24, *left++,  stats);
	sample1 = audio_linear_round(24, *right++, stats);

	data[0] = sample0 >> 16;
	data[1] = sample0 >>  8;
	data[2] = sample0 >>  0;

	data[3] = sample1 >> 16;
	data[4] = sample1 >>  8;
	data[5] = sample1 >>  0;

	data += 6;
      }
      break;

    case AUDIO_MODE_DITHER:
      while (len--) {
	sample0 = audio_linear_dither(24, *left++,  &left_dither,  stats);
	sample1 = audio_linear_dither(24, *right++, &right_dither, stats);

	data[0] = sample0 >> 16;
	data[1] = sample0 >>  8;
	data[2] = sample0 >>  0;

	data[3] = sample1 >> 16;
	data[4] = sample1 >>  8;
	data[5] = sample1 >>  0;

	data += 6;
      }
      break;

    default:
      return 0;
    }

    return nsamples * 3 * 2;
  }
  else {  /* mono */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--) {
	sample0 = audio_linear_round(24, *left++, stats);

	data[0] = sample0 >> 16;
	data[1] = sample0 >>  8;
	data[2] = sample0 >>  0;

	data += 3;
      }
      break;

    case AUDIO_MODE_DITHER:
      while (len--) {
	sample1 = audio_linear_dither(24, *left++, &left_dither, stats);

	data[0] = sample1 >> 16;
	data[1] = sample1 >>  8;
	data[2] = sample1 >>  0;

	data += 3;
      }
      break;

    default:
      return 0;
    }

    return nsamples * 3;
  }
}

/*
 * NAME:	audio_pcm_s32le()
 * DESCRIPTION:	write a block of signed 32-bit little-endian PCM samples
 */
unsigned int audio_pcm_s32le(unsigned char *data, unsigned int nsamples,
			     mad_fixed_t const *left, mad_fixed_t const *right,
			     enum audio_mode mode, struct audio_stats *stats)
{
  unsigned int len;
  register signed long sample0, sample1;

  len = nsamples;

  if (right) {  /* stereo */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--) {
	sample0 = audio_linear_round(24, *left++,  stats);
	sample1 = audio_linear_round(24, *right++, stats);

	data[0] = 0;
	data[1] = sample0 >>  0;
	data[2] = sample0 >>  8;
	data[3] = sample0 >> 16;

	data[4] = 0;
	data[5] = sample1 >>  0;
	data[6] = sample1 >>  8;
	data[7] = sample1 >> 16;

	data += 8;
      }
      break;

    case AUDIO_MODE_DITHER:
      while (len--) {
	sample0 = audio_linear_dither(24, *left++,  &left_dither,  stats);
	sample1 = audio_linear_dither(24, *right++, &right_dither, stats);

	data[0] = 0;
	data[1] = sample0 >>  0;
	data[2] = sample0 >>  8;
	data[3] = sample0 >> 16;

	data[4] = 0;
	data[5] = sample1 >>  0;
	data[6] = sample1 >>  8;
	data[7] = sample1 >> 16;

	data += 8;
      }
      break;

    default:
      return 0;
    }

    return nsamples * 4 * 2;
  }
  else {  /* mono */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--) {
	sample0 = audio_linear_round(24, *left++, stats);

	data[0] = 0;
	data[1] = sample0 >>  0;
	data[2] = sample0 >>  8;
	data[3] = sample0 >> 16;

	data += 4;
      }
      break;

    case AUDIO_MODE_DITHER:
      while (len--) {
	sample0 = audio_linear_dither(24, *left++, &left_dither, stats);

	data[0] = 0;
	data[1] = sample0 >>  0;
	data[2] = sample0 >>  8;
	data[3] = sample0 >> 16;

	data += 4;
      }
      break;

    default:
      return 0;
    }

    return nsamples * 4;
  }
}

/*
 * NAME:	audio_pcm_s32be()
 * DESCRIPTION:	write a block of signed 32-bit big-endian PCM samples
 */
unsigned int audio_pcm_s32be(unsigned char *data, unsigned int nsamples,
			     mad_fixed_t const *left, mad_fixed_t const *right,
			     enum audio_mode mode, struct audio_stats *stats)
{
  unsigned int len;
  register signed long sample0, sample1;

  len = nsamples;

  if (right) {  /* stereo */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--) {
	sample0 = audio_linear_round(24, *left++,  stats);
	sample1 = audio_linear_round(24, *right++, stats);

	data[0] = sample0 >> 16;
	data[1] = sample0 >>  8;
	data[2] = sample0 >>  0;
	data[3] = 0;

	data[4] = sample1 >> 16;
	data[5] = sample1 >>  8;
	data[6] = sample1 >>  0;
	data[7] = 0;

	data += 8;
      }
      break;

    case AUDIO_MODE_DITHER:
      while (len--) {
	sample0 = audio_linear_dither(24, *left++,  &left_dither,  stats);
	sample1 = audio_linear_dither(24, *right++, &right_dither, stats);

	data[0] = sample0 >> 16;
	data[1] = sample0 >>  8;
	data[2] = sample0 >>  0;
	data[3] = 0;

	data[4] = sample1 >> 16;
	data[5] = sample1 >>  8;
	data[6] = sample1 >>  0;
	data[7] = 0;

	data += 8;
      }
      break;

    default:
      return 0;
    }

    return nsamples * 4 * 2;
  }
  else {  /* mono */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--) {
	sample0 = audio_linear_round(24, *left++, stats);

	data[0] = sample0 >> 16;
	data[1] = sample0 >>  8;
	data[2] = sample0 >>  0;
	data[3] = 0;

	data += 4;
      }
      break;

    case AUDIO_MODE_DITHER:
      while (len--) {
	sample0 = audio_linear_dither(24, *left++, &left_dither, stats);

	data[0] = sample0 >> 16;
	data[1] = sample0 >>  8;
	data[2] = sample0 >>  0;
	data[3] = 0;

	data += 4;
      }
      break;

    default:
      return 0;
    }

    return nsamples * 4;
  }
}

static
unsigned char linear2mulaw(mad_fixed_t sample)
{
  unsigned int sign, mulaw;

  enum {
    BIAS = (mad_fixed_t) ((0x10 << 1) + 1) << (MAD_F_FRACBITS - 13)
  };

  if (sample < 0) {
    sample = BIAS - sample;
    sign   = 0x7f;
  }
  else {
    sample = BIAS + sample;
    sign   = 0xff;
  }

  mulaw = 0x7f;
  if (sample < MAD_F_ONE) {
    unsigned int segment;
    unsigned long mask;

    segment = 7;
    for (mask = 1L << (MAD_F_FRACBITS - 1); !(sample & mask); mask >>= 1)
      --segment;

    mulaw = ((segment << 4) |
	     ((sample >> (MAD_F_FRACBITS - 1 - (7 - segment) - 4)) & 0x0f));
  }

  mulaw ^= sign;

# if 0
  if (mulaw == 0x00)
    mulaw = 0x02;
# endif

  return mulaw;
}

static
mad_fixed_t mulaw2linear(unsigned char mulaw)
{
  int sign, segment, mantissa, value;

  enum {
    BIAS = (0x10 << 1) + 1
  };

  mulaw    = ~mulaw;
  sign     = (mulaw >> 7) & 0x01;
  segment  = (mulaw >> 4) & 0x07;
  mantissa = (mulaw >> 0) & 0x0f;

  value = ((0x21 | (mantissa << 1)) << segment) - BIAS;
  if (sign)
    value = -value;

  return (mad_fixed_t) value << (MAD_F_FRACBITS - 13);
}

/*
 * NAME:	audio_mulaw_round()
 * DESCRIPTION:	convert a linear PCM value to 8-bit ISDN mu-law
 */
inline
unsigned char audio_mulaw_round(mad_fixed_t sample, struct audio_stats *stats)
{
  clip(&sample, stats);

  return linear2mulaw(sample);
}

/*
 * NAME:	audio_mulaw_dither()
 * DESCRIPTION:	convert a linear PCM value to dithered 8-bit ISDN mu-law
 */
inline
unsigned char audio_mulaw_dither(mad_fixed_t sample,
				 struct audio_dither *dither,
				 struct audio_stats *stats)
{
  unsigned char mulaw;

  /* noise shape */
  sample += dither->error[0];

  clip(&sample, stats);

  mulaw = linear2mulaw(sample);

  /* error feedback */
  dither->error[0] = sample - mulaw2linear(mulaw);

  return mulaw;
}

/*
 * NAME:	audio_pcm_mulaw()
 * DESCRIPTION:	write a block of 8-bit mu-law encoded samples
 */
unsigned int audio_pcm_mulaw(unsigned char *data, unsigned int nsamples,
			     mad_fixed_t const *left, mad_fixed_t const *right,
			     enum audio_mode mode, struct audio_stats *stats)
{
  unsigned int len;

  len = nsamples;

  if (right) {  /* stereo */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--) {
	data[0] = audio_mulaw_round(*left++,  stats);
	data[1] = audio_mulaw_round(*right++, stats);

	data += 2;
      }
      break;

    case AUDIO_MODE_DITHER:
      while (len--) {
	data[0] = audio_mulaw_dither(*left++,  &left_dither,  stats);
	data[1] = audio_mulaw_dither(*right++, &right_dither, stats);

	data += 2;
      }
      break;

    default:
      return 0;
    }

    return nsamples * 2;
  }
  else {  /* mono */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--)
	*data++ = audio_mulaw_round(*left++, stats);
      break;

    case AUDIO_MODE_DITHER:
      while (len--)
	*data++ = audio_mulaw_dither(*left++, &left_dither, stats);
      break;

    default:
      return 0;
    }

    return nsamples;
  }
}
