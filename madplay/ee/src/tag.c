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
# include <stdio.h>
# include <mad.h>

# include "crc.h"
# include "rgain.h"
# include "tag.h"

# define XING_MAGIC	(('X' << 24) | ('i' << 16) | ('n' << 8) | 'g')
# define INFO_MAGIC	(('I' << 24) | ('n' << 16) | ('f' << 8) | 'o')
# define LAME_MAGIC	(('L' << 24) | ('A' << 16) | ('M' << 8) | 'E')

/*
 * NAME:	tag->init()
 * DESCRIPTION:	initialize tag structure
 */
void tag_init(struct tag *tag)
{
  tag->flags      = 0;
  tag->encoder[0] = 0;
}

/*
 * NAME:	parse_xing()
 * DESCRIPTION:	parse a Xing VBR tag
 */
static
int parse_xing(struct tag_xing *xing,
	       struct mad_bitptr *ptr, unsigned int *bitlen)
{
  if (*bitlen < 32)
    goto fail;

  xing->flags = mad_bit_read(ptr, 32);
  *bitlen -= 32;

  if (xing->flags & TAG_XING_FRAMES) {
    if (*bitlen < 32)
      goto fail;

    xing->frames = mad_bit_read(ptr, 32);
    *bitlen -= 32;
  }

  if (xing->flags & TAG_XING_BYTES) {
    if (*bitlen < 32)
      goto fail;

    xing->bytes = mad_bit_read(ptr, 32);
    *bitlen -= 32;
  }

  if (xing->flags & TAG_XING_TOC) {
    int i;

    if (*bitlen < 800)
      goto fail;

    for (i = 0; i < 100; ++i)
      xing->toc[i] = mad_bit_read(ptr, 8);

    *bitlen -= 800;
  }

  if (xing->flags & TAG_XING_SCALE) {
    if (*bitlen < 32)
      goto fail;

    xing->scale = mad_bit_read(ptr, 32);
    *bitlen -= 32;
  }

  return 0;

 fail:
  xing->flags = 0;
  return -1;
}

/*
 * NAME:	parse_lame()
 * DESCRIPTION:	parse a LAME tag
 */
static
int parse_lame(struct tag_lame *lame,
	       struct mad_bitptr *ptr, unsigned int *bitlen,
	       unsigned short crc)
{
  struct mad_bitptr save = *ptr;
  unsigned long magic;
  char const *version;

  if (*bitlen < 36 * 8)
    goto fail;

  /* bytes $9A-$A4: Encoder short VersionString */

  magic   = mad_bit_read(ptr, 4 * 8);
  version = mad_bit_nextbyte(ptr);

  mad_bit_skip(ptr, 5 * 8);

  /* byte $A5: Info Tag revision + VBR method */

  lame->revision = mad_bit_read(ptr, 4);
  if (lame->revision == 15)
    goto fail;

  lame->vbr_method = mad_bit_read(ptr, 4);

  /* byte $A6: Lowpass filter value (Hz) */

  lame->lowpass_filter = mad_bit_read(ptr, 8) * 100;

  /* bytes $A7-$AA: 32 bit "Peak signal amplitude" */

  lame->peak = mad_bit_read(ptr, 32) << 5;

  /* bytes $AB-$AC: 16 bit "Radio Replay Gain" */

  rgain_parse(&lame->replay_gain[0], ptr);

  /* bytes $AD-$AE: 16 bit "Audiophile Replay Gain" */

  rgain_parse(&lame->replay_gain[1], ptr);

  /*
   * As of version 3.95.1, LAME writes Replay Gain values with a reference of
   * 89 dB SPL instead of the 83 dB specified in the Replay Gain proposed
   * standard. Here we compensate for the heresy.
   */
  if (magic == LAME_MAGIC) {
    char str[6];
    unsigned major = 0, minor = 0, patch = 0;
    int i;

    memcpy(str, version, 5);
    str[5] = 0;

//    sscanf(str, "%u.%u.%u", &major, &minor, &patch);
    printf ("fixme: parse_lame\n");

    if (major > 3 ||
	(major == 3 && (minor > 95 ||
			(minor == 95 && str[4] == '.')))) {
      for (i = 0; i < 2; ++i) {
	if (RGAIN_SET(&lame->replay_gain[i]))
	  lame->replay_gain[i].adjustment -= 60;  /* 6.0 dB */
      }
    }
  }

  /* byte $AF: Encoding flags + ATH Type */

  lame->flags    = mad_bit_read(ptr, 4);
  lame->ath_type = mad_bit_read(ptr, 4);

  /* byte $B0: if ABR {specified bitrate} else {minimal bitrate} */

  lame->bitrate = mad_bit_read(ptr, 8);

  /* bytes $B1-$B3: Encoder delays */

  lame->start_delay = mad_bit_read(ptr, 12);
  lame->end_padding = mad_bit_read(ptr, 12);

  /* byte $B4: Misc */

  lame->source_samplerate = mad_bit_read(ptr, 2);

  if (mad_bit_read(ptr, 1))
    lame->flags |= TAG_LAME_UNWISE;

  lame->stereo_mode   = mad_bit_read(ptr, 3);
  lame->noise_shaping = mad_bit_read(ptr, 2);

  /* byte $B5: MP3 Gain */

  lame->gain = mad_bit_read(ptr, 8);

  /* bytes $B6-B7: Preset and surround info */

  mad_bit_skip(ptr, 2);

  lame->surround = mad_bit_read(ptr,  3);
  lame->preset   = mad_bit_read(ptr, 11);

  /* bytes $B8-$BB: MusicLength */

  lame->music_length = mad_bit_read(ptr, 32);

  /* bytes $BC-$BD: MusicCRC */

  lame->music_crc = mad_bit_read(ptr, 16);

  /* bytes $BE-$BF: CRC-16 of Info Tag */

  if (mad_bit_read(ptr, 16) != crc)
    goto fail;

  *bitlen -= 36 * 8;

  return 0;

 fail:
  *ptr = save;
  return -1;
}

/*
 * NAME:	tag->parse()
 * DESCRIPTION:	parse Xing/LAME tag(s)
 */
int tag_parse(struct tag *tag, struct mad_stream const *stream)
{
  struct mad_bitptr ptr = stream->anc_ptr;
  struct mad_bitptr start = ptr;
  unsigned int bitlen = stream->anc_bitlen;
  unsigned long magic;
  int i;

  if (bitlen < 32)
    return -1;

  magic = mad_bit_read(&ptr, 32);
  bitlen -= 32;

  if (magic != XING_MAGIC &&
      magic != INFO_MAGIC &&
      magic != LAME_MAGIC) {
    /*
     * Due to an unfortunate historical accident, a Xing VBR tag may be
     * misplaced in a stream with CRC protection. We check for this by
     * assuming the tag began two octets prior and the high bits of the
     * following flags field are always zero.
     */

    if (magic != ((XING_MAGIC << 16) & 0xffffffffL) &&
	magic != ((INFO_MAGIC << 16) & 0xffffffffL))
      return -1;

    magic >>= 16;

    /* backtrack the bit pointer */

    ptr = start;
    mad_bit_skip(&ptr, 16);
    bitlen += 16;
  }

  if ((magic & 0x0000ffffL) == (XING_MAGIC & 0x0000ffffL))
    tag->flags |= TAG_VBR;

  /* Xing tag */

  if (magic == LAME_MAGIC) {
    ptr = start;
    bitlen += 32;
  }
  else if (parse_xing(&tag->xing, &ptr, &bitlen) == 0)
    tag->flags |= TAG_XING;

  /* encoder string */

  if (bitlen >= 20 * 8) {
    start = ptr;

    for (i = 0; i < 20; ++i) {
      tag->encoder[i] = mad_bit_read(&ptr, 8);

      if (tag->encoder[i] == 0)
	break;

      /* keep only printable ASCII chars */

      if (tag->encoder[i] < 0x20 || tag->encoder[i] >= 0x7f) {
	tag->encoder[i] = 0;
	break;
      }
    }

    tag->encoder[20] = 0;
    ptr = start;
  }

  /* LAME tag */

  if (stream->next_frame - stream->this_frame >= 192 &&
      parse_lame(&tag->lame, &ptr, &bitlen,
		 crc_compute(stream->this_frame, 190, 0x0000)) == 0) {
    tag->flags |= TAG_LAME;
    tag->encoder[9] = 0;
  }
  else {
    for (i = 0; i < 20; ++i) {
      if (tag->encoder[i] == 0)
	break;

      /* stop at padding chars */

      if (tag->encoder[i] == 0x55) {
	tag->encoder[i] = 0;
	break;
      }
    }
  }

  return 0;
}
