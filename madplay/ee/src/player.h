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

# ifndef PLAYER_H
# define PLAYER_H

# include <stdio.h>
# include <mad.h>

# include "audio.h"
# include "resample.h"
# include "filter.h"
# include "tag.h"

enum {
  PLAYER_OPTION_SHUFFLE      = 0x0001,
  PLAYER_OPTION_DOWNSAMPLE   = 0x0002,
  PLAYER_OPTION_IGNORECRC    = 0x0004,
  PLAYER_OPTION_IGNOREVOLADJ = 0x0008,

  PLAYER_OPTION_SKIP         = 0x0010,
  PLAYER_OPTION_TIMED        = 0x0020,
  PLAYER_OPTION_TTYCONTROL   = 0x0040,
  PLAYER_OPTION_STREAMID3    = 0x0080,

  PLAYER_OPTION_FADEIN       = 0x0100,
  PLAYER_OPTION_FADEOUT      = 0x0200,
  PLAYER_OPTION_GAP          = 0x0400,
  PLAYER_OPTION_CROSSFADE    = 0x0800,

# if defined(EXPERIMENTAL)
  PLAYER_OPTION_EXTERNALMIX  = 0x1000,
  PLAYER_OPTION_EXPERIMENTAL = 0x2000,
# endif

  PLAYER_OPTION_SHOWTAGSONLY = 0x4000
};

enum player_control {
  PLAYER_CONTROL_DEFAULT,
  PLAYER_CONTROL_NEXT,
  PLAYER_CONTROL_PREVIOUS,
  PLAYER_CONTROL_REPLAY,
  PLAYER_CONTROL_STOP
};

enum player_channel {
  PLAYER_CHANNEL_DEFAULT = 0,
  PLAYER_CHANNEL_LEFT    = 1,
  PLAYER_CHANNEL_RIGHT   = 2,
  PLAYER_CHANNEL_MONO    = 3,
  PLAYER_CHANNEL_STEREO  = 4
};

enum stats_show {
  STATS_SHOW_OVERALL,
  STATS_SHOW_CURRENT,
  STATS_SHOW_REMAINING
};

enum {
  DB_MIN = -175,	/* minimum representable mad_fixed_t factor */
  DB_MAX =  +18		/* maximum representable mad_fixed_t factor */
};

enum {
  PLAYER_RGAIN_ENABLED    = 0x0001,
  PLAYER_RGAIN_SET        = 0x0002,
  PLAYER_RGAIN_AUDIOPHILE = 0x0010,
  PLAYER_RGAIN_HARDLIMIT  = 0x0020
};

struct player {
  int verbosity;

  int options;
  int repeat;

  enum player_control control;

  struct playlist {
    char const **entries;
    int length;
    int current;
  } playlist;

  mad_timer_t global_start;
  mad_timer_t global_stop;

  mad_timer_t fade_in;
  mad_timer_t fade_out;
  mad_timer_t gap;

  struct input {
    char const *path;

    int fd;
# if defined(HAVE_MMAP)
    unsigned char *fdm;
# endif

    unsigned char *data;
    unsigned long length;

    int eof;

    struct tag tag;
  } input;

  struct output {
    enum audio_mode mode;

    double voladj_db;
    double attamp_db;
    mad_fixed_t gain;

    int replay_gain;

    struct filter *filters;

    unsigned int channels_in;
    unsigned int channels_out;
    enum player_channel select;

    unsigned int speed_in;
    unsigned int speed_out;
    unsigned int speed_request;

    unsigned int precision_in;
    unsigned int precision_out;

    char const *path;
    audio_ctlfunc_t *command;

    struct resample_state resample[2];
    mad_fixed_t (*resampled)[2][MAX_NSAMPLES];
  } output;

  struct ancillary {
    char const *path;
    FILE *file;

    unsigned short buffer;
    unsigned short length;
  } ancillary;

  struct stats {
    enum stats_show show;
    char const *label;

    unsigned long total_bytes;
    mad_timer_t total_time;

    mad_timer_t global_timer;
    mad_timer_t absolute_timer;
    mad_timer_t play_timer;

    unsigned long global_framecount;
    unsigned long absolute_framecount;
    unsigned long play_framecount;

    unsigned long error_frame;
    unsigned long mute_frame;

    int vbr;
    unsigned int bitrate;
    unsigned long vbr_frames;
    unsigned long vbr_rate;

    signed long nsecs;

    struct audio_stats audio;
  } stats;
};

void player_init(struct player *);
void player_finish(struct player *);

int player_run(struct player *, int, char const *[]);

# endif
