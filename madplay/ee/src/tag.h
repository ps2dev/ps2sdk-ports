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

# ifndef TAG_H
# define TAG_H

# include <mad.h>

# include "rgain.h"

enum {
  TAG_XING = 0x0001,
  TAG_LAME = 0x0002,
  TAG_VBR  = 0x0100
};

enum {
  TAG_XING_FRAMES = 0x00000001L,
  TAG_XING_BYTES  = 0x00000002L,
  TAG_XING_TOC    = 0x00000004L,
  TAG_XING_SCALE  = 0x00000008L
};

struct tag_xing {
  long flags;		   /* valid fields (see above) */
  unsigned long frames;	   /* total number of frames */
  unsigned long bytes;	   /* total number of bytes */
  unsigned char toc[100];  /* 100-point seek table */
  long scale;		   /* VBR quality indicator (0 best - 100 worst) */
};

enum {
  TAG_LAME_NSPSYTUNE    = 0x01,
  TAG_LAME_NSSAFEJOINT  = 0x02,
  TAG_LAME_NOGAP_NEXT   = 0x04,
  TAG_LAME_NOGAP_PREV   = 0x08,

  TAG_LAME_UNWISE       = 0x10
};

enum tag_lame_vbr {
  TAG_LAME_VBR_CONSTANT      = 1,
  TAG_LAME_VBR_ABR           = 2,
  TAG_LAME_VBR_METHOD1       = 3,
  TAG_LAME_VBR_METHOD2       = 4,
  TAG_LAME_VBR_METHOD3       = 5,
  TAG_LAME_VBR_METHOD4       = 6,
  TAG_LAME_VBR_CONSTANT2PASS = 8,
  TAG_LAME_VBR_ABR2PASS      = 9
};

enum tag_lame_source {
  TAG_LAME_SOURCE_32LOWER  = 0x00,
  TAG_LAME_SOURCE_44_1     = 0x01,
  TAG_LAME_SOURCE_48       = 0x02,
  TAG_LAME_SOURCE_HIGHER48 = 0x03
};

enum tag_lame_mode {
  TAG_LAME_MODE_MONO      = 0x00,
  TAG_LAME_MODE_STEREO    = 0x01,
  TAG_LAME_MODE_DUAL      = 0x02,
  TAG_LAME_MODE_JOINT     = 0x03,
  TAG_LAME_MODE_FORCE     = 0x04,
  TAG_LAME_MODE_AUTO      = 0x05,
  TAG_LAME_MODE_INTENSITY = 0x06,
  TAG_LAME_MODE_UNDEFINED = 0x07
};

enum tag_lame_surround {
  TAG_LAME_SURROUND_NONE      = 0,
  TAG_LAME_SURROUND_DPL       = 1,
  TAG_LAME_SURROUND_DPL2      = 2,
  TAG_LAME_SURROUND_AMBISONIC = 3
};

enum tag_lame_preset {
  TAG_LAME_PRESET_NONE          =    0,
  TAG_LAME_PRESET_V9            =  410,
  TAG_LAME_PRESET_V8            =  420,
  TAG_LAME_PRESET_V7            =  430,
  TAG_LAME_PRESET_V6            =  440,
  TAG_LAME_PRESET_V5            =  450,
  TAG_LAME_PRESET_V4            =  460,
  TAG_LAME_PRESET_V3            =  470,
  TAG_LAME_PRESET_V2            =  480,
  TAG_LAME_PRESET_V1            =  490,
  TAG_LAME_PRESET_V0            =  500,
  TAG_LAME_PRESET_R3MIX         = 1000,
  TAG_LAME_PRESET_STANDARD      = 1001,
  TAG_LAME_PRESET_EXTREME       = 1002,
  TAG_LAME_PRESET_INSANE        = 1003,
  TAG_LAME_PRESET_STANDARD_FAST = 1004,
  TAG_LAME_PRESET_EXTREME_FAST  = 1005,
  TAG_LAME_PRESET_MEDIUM        = 1006,
  TAG_LAME_PRESET_MEDIUM_FAST   = 1007
};

struct tag_lame {
  unsigned char revision;
  unsigned char flags;

  enum tag_lame_vbr vbr_method;
  unsigned short lowpass_filter;

  mad_fixed_t peak;
  struct rgain replay_gain[2];

  unsigned char ath_type;
  unsigned char bitrate;

  unsigned short start_delay;
  unsigned short end_padding;

  enum tag_lame_source source_samplerate;
  enum tag_lame_mode stereo_mode;
  unsigned char noise_shaping;

  signed char gain;
  enum tag_lame_surround surround;
  enum tag_lame_preset preset;

  unsigned long music_length;
  unsigned short music_crc;
};

struct tag {
  int flags;
  struct tag_xing xing;
  struct tag_lame lame;
  char encoder[21];
};

void tag_init(struct tag *);

# define tag_finish(tag)	/* nothing */

int tag_parse(struct tag *, struct mad_stream const *);

# endif
