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
# include <stdarg.h>
# include <stdlib.h>

# ifdef HAVE_SYS_TYPES_H
#  include <sys/types.h>
# endif

# include <sys/stat.h>

# ifdef HAVE_FCNTL_H
#  include <fcntl.h>
# endif

# ifdef HAVE_UNISTD_H
#  include <unistd.h>
# endif

# include <string.h>

# ifdef HAVE_ERRNO_H
#  include <errno.h>
# endif

# include <time.h>
# include <locale.h>
# include <math.h>

# ifdef HAVE_TERMIOS_H
#  include <termios.h>
# endif

# ifdef _WIN32
#  include <windows.h>
# endif

# include <signal.h>

# ifdef HAVE_ASSERT_H
#  include <assert.h>
# endif

# if defined(HAVE_MMAP)
#  include <sys/mman.h>
# endif

# if !defined(O_BINARY)
#  define O_BINARY  0
# endif

# include <mad.h>
# include <id3tag.h>

# include "gettext.h"

# include "player.h"
# include "audio.h"
# include "resample.h"
# include "filter.h"
# include "tag.h"
# include "rgain.h"

# include "bstdfile.h"

# define MPEG_BUFSZ	80000	/* 5.0 s at 128 kbps; 2 s at 320 kbps */
# define FREQ_TOLERANCE	2	/* percent sampling frequency tolerance */

# define TTY_DEVICE	"/dev/tty"

# define KEY_CTRL(key)	((key) & 0x1f)

enum {
  KEY_PAUSE    = 'p',
  KEY_STOP     = 's',
  KEY_FORWARD  = 'f',
  KEY_BACK     = 'b',
  KEY_TIME     = 't',
  KEY_QUIT     = 'q',
  KEY_INFO     = 'i',
  KEY_GAINDECR = '-',
  KEY_GAININCR = '+',
  KEY_GAINZERO = '_',
  KEY_GAININFO = '='
};

static int on_same_line __attribute__((aligned (16)));
bstdfile_t	*BstdFile __attribute__((aligned (16)));

# if defined(USE_TTY) && !defined(_WIN32)
static int tty_fd = -1;
static struct termios save_tty;
static struct sigaction save_sigtstp, save_sigint;
# endif

/*
 * NAME:	player_init()
 * DESCRIPTION:	initialize player structure
 */
void player_init(struct player *player)
{
  player->verbosity = 0;

  player->options = 0;
  player->repeat  = 1;

  player->control = PLAYER_CONTROL_DEFAULT;

  player->playlist.entries = 0;
  player->playlist.length  = 0;
  player->playlist.current = 0;

  player->global_start = mad_timer_zero;
  player->global_stop  = mad_timer_zero;

  player->fade_in      = mad_timer_zero;
  player->fade_out     = mad_timer_zero;
  player->gap          = mad_timer_zero;

  player->input.path   = 0;
  player->input.fd     = -1;
# if defined(HAVE_MMAP)
  player->input.fdm    = 0;
# endif
  player->input.data   = 0;
  player->input.length = 0;
  player->input.eof    = 0;

  tag_init(&player->input.tag);

  player->output.mode          = AUDIO_MODE_DITHER;
  player->output.voladj_db     = 0;
  player->output.attamp_db     = 0;
  player->output.gain          = MAD_F_ONE;
  player->output.replay_gain   = 0;
  player->output.filters       = 0;
  player->output.channels_in   = 0;
  player->output.channels_out  = 0;
  player->output.select        = PLAYER_CHANNEL_DEFAULT;
  player->output.speed_in      = 0;
  player->output.speed_out     = 0;
  player->output.speed_request = 0;
  player->output.precision_in  = 0;
  player->output.precision_out = 0;
  player->output.path          = 0;
  player->output.command       = 0;
  /* player->output.resample */
  player->output.resampled     = 0;

  player->ancillary.path       = 0;
  player->ancillary.file       = 0;
  player->ancillary.buffer     = 0;
  player->ancillary.length     = 0;

  player->stats.show                  = STATS_SHOW_OVERALL;
  player->stats.label                 = 0;
  player->stats.total_bytes           = 0;
  player->stats.total_time            = mad_timer_zero;
  player->stats.global_timer          = mad_timer_zero;
  player->stats.absolute_timer        = mad_timer_zero;
  player->stats.play_timer            = mad_timer_zero;
  player->stats.global_framecount     = 0;
  player->stats.absolute_framecount   = 0;
  player->stats.play_framecount       = 0;
  player->stats.error_frame           = -1;
  player->stats.mute_frame            = 0;
  player->stats.vbr                   = 0;
  player->stats.bitrate               = 0;
  player->stats.vbr_frames            = 0;
  player->stats.vbr_rate              = 0;
  player->stats.nsecs                 = 0;
  player->stats.audio.clipped_samples = 0;
  player->stats.audio.peak_clipping   = 0;
  player->stats.audio.peak_sample     = 0;
}

/*
 * NAME:	player_finish()
 * DESCRIPTION:	destroy a player structure
 */
void player_finish(struct player *player)
{
  if (player->output.filters)
    filter_free(player->output.filters);

  if (player->output.resampled) {
    resample_finish(&player->output.resample[0]);
    resample_finish(&player->output.resample[1]);

    free(player->output.resampled);
    player->output.resampled = 0;
  }
}

/*
 * NAME:	message()
 * DESCRIPTION:	show a message, possibly overwriting a previous w/o newline
 */
static
int message(char const *format, ...)
{
  int len, newline, result;
  va_list args;

  len = strlen(format);
  newline = (len > 0 && format[len - 1] == '\n');

  if (on_same_line && newline && len > 1)
    fputc('\n', stderr);

  va_start(args, format);
  result = vfprintf(stderr, format, args);
  va_end(args);

  if (on_same_line && !newline && result < on_same_line) {
    unsigned int i;

    i = on_same_line - result;
    while (i--)
      putc(' ', stderr);
  }

  on_same_line = newline ? 0 : result;

  if (!newline) {
    fputc('\r', stderr);
    fflush(stderr);
  }

  return result;
}

/*
 * NAME:	detail()
 * DESCRIPTION:	show right-aligned label and line-wrap corresponding text
 */
static
void detail(char const *label, char const *format, ...)
{
  char const spaces[] = "               ";
  va_list args;

# define LINEWRAP  (80 - sizeof(spaces) - 2 - 2)

  if (on_same_line)
    message("\n");

  if (label) {
    unsigned int len;

    len = strlen(label);
    assert(len < sizeof(spaces));

    fprintf(stderr, "%s%s: ", &spaces[len], label);
  }
  else
    fprintf(stderr, "%s  ", spaces);

  va_start(args, format);

  if (format) {
    vfprintf(stderr, format, args);
    fputc('\n', stderr);
  }
  else {
    char *ptr, *newline, *linebreak;

    /* N.B. this argument must be mutable! */
    ptr = va_arg(args, char *);

    do {
      newline = strchr(ptr, '\n');
      if (newline)
	*newline = 0;

      if (strlen(ptr) > LINEWRAP) {
	linebreak = ptr + LINEWRAP;

	while (linebreak > ptr && *linebreak != ' ')
	  --linebreak;

	if (*linebreak == ' ') {
	  if (newline)
	    *newline = '\n';

	  *(newline = linebreak) = 0;
	}
      }

      fprintf(stderr, "%s\n", ptr);

      if (newline) {
	ptr = newline + 1;
	fprintf(stderr, "%s  ", spaces);
      }
    }
    while (newline);
  }

  va_end(args);
}

/*
 * NAME:	error()
 * DESCRIPTION:	show an error using proper interaction with message()
 */
static
void error(char const *id, char const *format, ...)
{
  int err;
  va_list args;

  err = errno;

  if (on_same_line)
    message("\n");

  if (id)
    fprintf(stderr, "%s: ", id);

  va_start(args, format);

  if (*format == ':') {
    if (format[1] == 0) {
      format = va_arg(args, char const *);
      errno = err;
      perror(format);
    }
    else {
      errno = err;
      perror(format + 1);
    }
  }
  else {
    vfprintf(stderr, format, args);
    fputc('\n', stderr);
  }

  va_end(args);
}

# if defined(HAVE_MMAP)
/*
 * NAME:	map_file()
 * DESCRIPTION:	map the contents of a file into memory
 */
static
void *map_file(int fd, unsigned long length)
{
  void *fdm;

  fdm = mmap(0, length, PROT_READ, MAP_SHARED, fd, 0);
  if (fdm == MAP_FAILED)
    return 0;

# if defined(HAVE_MADVISE)
  madvise(fdm, length, MADV_SEQUENTIAL);
# endif

  return fdm;
}

/*
 * NAME:	unmap_file()
 * DESCRIPTION:	undo a file mapping
 */
static
int unmap_file(void *fdm, unsigned long length)
{
  if (munmap(fdm, length) == -1)
    return -1;

  return 0;
}

/*
 * NAME:	decode->input_mmap()
 * DESCRIPTION:	(re)fill decoder input buffer from a memory map
 */
static
enum mad_flow decode_input_mmap(void *data, struct mad_stream *stream)
{
  struct player *player = data;
  struct input *input = &player->input;

  if (input->eof)
    return MAD_FLOW_STOP;

  if (stream->next_frame) {
    struct stat s;
    unsigned long posn, left;

    if (fstat(input->fd, &s) == -1)
      return MAD_FLOW_BREAK;

    posn = stream->next_frame - input->fdm;

    /* check for file size change and update map */

    if (s.st_size > input->length) {
      if (unmap_file(input->fdm, input->length) == -1) {
	input->fdm  = 0;
	input->data = 0;
	return MAD_FLOW_BREAK;
      }

      player->stats.total_bytes += s.st_size - input->length;

      input->length = s.st_size;

      input->fdm = map_file(input->fd, input->length);
      if (input->fdm == 0) {
	input->data = 0;
	return MAD_FLOW_BREAK;
      }

      mad_stream_buffer(stream, input->fdm + posn, input->length - posn);

      return MAD_FLOW_CONTINUE;
    }

    /* end of memory map; append MAD_BUFFER_GUARD zero bytes */

    left = input->length - posn;

    input->data = malloc(left + MAD_BUFFER_GUARD);
    if (input->data == 0)
      return MAD_FLOW_BREAK;

    input->eof = 1;

    memcpy(input->data, input->fdm + posn, left);
    memset(input->data + left, 0, MAD_BUFFER_GUARD);

    mad_stream_buffer(stream, input->data, left + MAD_BUFFER_GUARD);

    return MAD_FLOW_CONTINUE;
  }

  /* first call */

  mad_stream_buffer(stream, input->fdm, input->length);

  return MAD_FLOW_CONTINUE;
}
# endif

/*
 * NAME:	decode->input_read()
 * DESCRIPTION:	(re)fill decoder input buffer by reading a file descriptor
 */
static
enum mad_flow decode_input_read(void *data, struct mad_stream *stream)
{
  struct player *player = data;
  struct input *input = &player->input;
  int len = 0;

//printf ("decode_input_read %p\n", data);

  if (input->eof)
    return MAD_FLOW_STOP;

  if (stream->next_frame) {
//printf ("	stream->next_frame\n");
    memmove(input->data, stream->next_frame,
	    input->length = &input->data[input->length] - stream->next_frame);
  }

  do {
//printf ("	read data\n");
//    len = read(input->fd, input->data + input->length,
//	       MPEG_BUFSZ - input->length);
      len = BstdRead(input->data + input->length,MPEG_BUFSZ - input->length,1,BstdFile);
  }
  while (len == -1 && errno == EINTR);

  if (len == -1) {
    error("input", ":read");
    return MAD_FLOW_BREAK;
  }
  else if (len == 0) {
//printf ("	len == 0\n");
    input->eof = 1;

    assert(MPEG_BUFSZ - input->length >= MAD_BUFFER_GUARD);

    while (len < MAD_BUFFER_GUARD)
      input->data[input->length + len++] = 0;
  }

  mad_stream_buffer(stream, input->data, input->length += len);

  return MAD_FLOW_CONTINUE;
}

/*
 * NAME:	decode->header()
 * DESCRIPTION:	decide whether to continue decoding based on header
 */
static
enum mad_flow decode_header(void *data, struct mad_header const *header)
{
  struct player *player = data;

  if ((player->options & PLAYER_OPTION_TIMED) &&
      mad_timer_compare(player->stats.global_timer, player->global_stop) > 0)
    return MAD_FLOW_STOP;

  /* accounting (except first frame) */

  if (player->stats.absolute_framecount) {
    ++player->stats.absolute_framecount;
    mad_timer_add(&player->stats.absolute_timer, header->duration);

    ++player->stats.global_framecount;
    mad_timer_add(&player->stats.global_timer, header->duration);

    if ((player->options & PLAYER_OPTION_SKIP) &&
	mad_timer_compare(player->stats.global_timer,
			  player->global_start) < 0)
      return MAD_FLOW_IGNORE;
  }

  return MAD_FLOW_CONTINUE;
}

/*
 * NAME:	write_ancillary()
 * DESCRIPTION:	pack ancillary data into octets and output
 */
static
int write_ancillary(struct ancillary *ancillary,
		    struct mad_bitptr ptr, unsigned int length)
{
  if (ancillary->length) {
    unsigned int balance;

    balance = 8 - ancillary->length;
    if (balance > length) {
      ancillary->buffer =
	(ancillary->buffer << length) | mad_bit_read(&ptr, length);
      ancillary->length += length;

      return 0;
    }
    else {
      if (fputc((ancillary->buffer << balance) | mad_bit_read(&ptr, balance),
		ancillary->file) == EOF) {
	error("ancillary", ":fputc");
	return -1;
      }
      ancillary->length = 0;

      length -= balance;
    }
  }

  while (length >= 8) {
    int byte;

    byte = mad_bit_read(&ptr, 8);
    if (putc(byte, ancillary->file) == EOF) {
      error("ancillary", ":putc");
      return -1;
    }

    length -= 8;
  }

  if (length) {
    ancillary->buffer = mad_bit_read(&ptr, length);
    ancillary->length = length;
  }

  if (fflush(ancillary->file) == EOF) {
    error("ancillary", ":fflush");
    return -1;
  }

  return 0;
}

/*
 * NAME:	show_id3()
 * DESCRIPTION:	display ID3 tag information
 */
static
void show_id3(struct id3_tag const *tag)
{
  unsigned int i;
  struct id3_frame const *frame;
  id3_ucs4_t const *ucs4;
  id3_latin1_t *latin1;

  static struct {
    char const *id;
    char const *label;
  } const info[] = {
    { ID3_FRAME_TITLE,  N_("Title")     },
    { "TIT3",           0               },  /* Subtitle */
    { "TCOP",           0               },  /* Copyright */
    { "TPRO",           0               },  /* Produced */
    { "TCOM",           N_("Composer")  },
    { ID3_FRAME_ARTIST, N_("Artist")    },
    { "TPE2",           N_("Orchestra") },
    { "TPE3",           N_("Conductor") },
    { "TEXT",           N_("Lyricist")  },
    { ID3_FRAME_ALBUM,  N_("Album")     },
    { ID3_FRAME_TRACK,  N_("Track")     },
    { ID3_FRAME_YEAR,   N_("Year")      },
    { "TPUB",           N_("Publisher") },
    { ID3_FRAME_GENRE,  N_("Genre")     },
    { "TRSN",           N_("Station")   },
    { "TENC",           N_("Encoder")   }
  };

  /* text information */

  for (i = 0; i < sizeof(info) / sizeof(info[0]); ++i) {
    union id3_field const *field;
    unsigned int nstrings, j;

    frame = id3_tag_findframe(tag, info[i].id, 0);
    if (frame == 0)
      continue;

    field    = id3_frame_field(frame, 1);
    nstrings = id3_field_getnstrings(field);

    for (j = 0; j < nstrings; ++j) {
      ucs4 = id3_field_getstrings(field, j);
      assert(ucs4);

      if (strcmp(info[i].id, ID3_FRAME_GENRE) == 0)
	ucs4 = id3_genre_name(ucs4);

      latin1 = id3_ucs4_latin1duplicate(ucs4);
      if (latin1 == 0)
	goto fail;

      if (j == 0 && info[i].label)
	detail(gettext(info[i].label), 0, latin1);
      else {
	if (strcmp(info[i].id, "TCOP") == 0 ||
	    strcmp(info[i].id, "TPRO") == 0) {
	  detail(0, "%s %s", (info[i].id[1] == 'C') ?
		 _("Copyright (C)") : _("Produced (P)"), latin1);
	}
	else
	  detail(0, 0, latin1);
      }

      free(latin1);
    }
  }

  /* comments */

  i = 0;
  while ((frame = id3_tag_findframe(tag, ID3_FRAME_COMMENT, i++))) {
    ucs4 = id3_field_getstring(id3_frame_field(frame, 2));
    assert(ucs4);

    if (*ucs4)
      continue;

    ucs4 = id3_field_getfullstring(id3_frame_field(frame, 3));
    assert(ucs4);

    latin1 = id3_ucs4_latin1duplicate(ucs4);
    if (latin1 == 0)
      goto fail;

    detail(_("Comment"), 0, latin1);

    free(latin1);
    break;
  }

  if (0) {
  fail:
    error("id3", _("not enough memory to display tag"));
  }
}

/*
 * NAME:	show_rgain()
 * DESCRIPTION:	display Replay Gain information
 */
static
void show_rgain(struct rgain const *rgain)
{
  char const *label, *source;

  if (!RGAIN_VALID(rgain))
    return;

  label = 0;
  switch (rgain->name) {
  case RGAIN_NAME_NOT_SET:
    break;
  case RGAIN_NAME_RADIO:
    label = _("Radio Gain");
    break;
  case RGAIN_NAME_AUDIOPHILE:
    label = _("Audiophile Gain");
    break;
  }

  source = rgain_originator(rgain);

  assert(label && source);

  detail(label, "%+.1f dB => %d dB SPL (%s)",
	 RGAIN_DB(rgain), (int) RGAIN_REFERENCE, source);
}

/*
 * NAME:	show_tag()
 * DESCRIPTION:	display Xing/LAME tag information
 */
static
void show_tag(struct tag const *tag)
{
  char ident[22];
  int i;

  memcpy(ident, tag->encoder, 21);

  /* separate version number from encoder name */

  for (i = 0; i < 20; ++i) {
    if (ident[i] == 0)
      break;

    if (ident[i] >= '0' && ident[i] <= '9') {
      if (i > 0 && ident[i - 1] != ' ' && ident[i - 1] != 'v') {
	memmove(&ident[i + 1], &ident[i], 21 - i);
	ident[i] = ' ';
      }

      break;
    }
  }

  if (ident[0])
    detail(_("Encoder Version"), "%s", ident);

  if (tag->flags & TAG_LAME) {
    char const *text;

# if 0
    detail(_("Tag Revision"), "%u", tag->lame.revision);
# endif

    text = 0;
    switch (tag->lame.vbr_method) {
    case TAG_LAME_VBR_CONSTANT:
      text = _("constant");
      break;
    case TAG_LAME_VBR_ABR:
      text = _("ABR");
	break;
    case TAG_LAME_VBR_METHOD1:
      text = _("1 (old/rh)");
      break;
    case TAG_LAME_VBR_METHOD2:
      text = _("2 (mtrh)");
      break;
    case TAG_LAME_VBR_METHOD3:
      text = _("3 (mt)");
      break;
    case TAG_LAME_VBR_METHOD4:
      text = _("4");
      break;
    case TAG_LAME_VBR_CONSTANT2PASS:
      text = _("constant (two-pass)");
      break;
    case TAG_LAME_VBR_ABR2PASS:
      text = _("ABR (two-pass)");
      break;
    }
    detail(_("VBR Method"), "%s", text ? text : _("unknown"));

    text = 0;
    switch (tag->lame.vbr_method) {
    case TAG_LAME_VBR_CONSTANT:
    case TAG_LAME_VBR_CONSTANT2PASS:
      text = _("Bitrate");
      break;
    case TAG_LAME_VBR_ABR:
    case TAG_LAME_VBR_ABR2PASS:
      text = _("Target Bitrate");
      break;
    case TAG_LAME_VBR_METHOD1:
    case TAG_LAME_VBR_METHOD2:
    case TAG_LAME_VBR_METHOD3:
    case TAG_LAME_VBR_METHOD4:
      text = _("Minimum Bitrate");
      break;
    }
    if (text) {
      detail(text, _("%u%s kbps"), tag->lame.bitrate,
	     tag->lame.bitrate == 255 ? "+" : "");
    }

    text = 0;
    switch (tag->lame.stereo_mode) {
    case TAG_LAME_MODE_MONO:
      text = _("mono");
      break;
    case TAG_LAME_MODE_STEREO:
      text = _("normal");
      break;
    case TAG_LAME_MODE_DUAL:
      text = _("dual channel");
      break;
    case TAG_LAME_MODE_JOINT:
      text = _("joint");
      break;
    case TAG_LAME_MODE_FORCE:
      text = _("force");
      break;
    case TAG_LAME_MODE_AUTO:
      text = _("auto");
      break;
    case TAG_LAME_MODE_INTENSITY:
      text = _("intensity");
      break;
    case TAG_LAME_MODE_UNDEFINED:
      text = _("undefined");
      break;
    }
    if (text)
      detail(_("Stereo Mode"), "%s", text);

    if (tag->lame.preset >= 8 && tag->lame.preset <= 320)
      detail(_("Preset"), _("ABR %u"), tag->lame.preset);
    else {
      text = 0;
      switch (tag->lame.preset) {
      case TAG_LAME_PRESET_NONE:
	text = _("none");
	break;
      case TAG_LAME_PRESET_V9:
	text = _("V9");
	break;
      case TAG_LAME_PRESET_V8:
	text = _("V8");
	break;
      case TAG_LAME_PRESET_V7:
	text = _("V7");
	break;
      case TAG_LAME_PRESET_V6:
	text = _("V6");
	break;
      case TAG_LAME_PRESET_V5:
	text = _("V5");
	break;
      case TAG_LAME_PRESET_V4:
	text = _("V4");
	break;
      case TAG_LAME_PRESET_V3:
	text = _("V3");
	break;
      case TAG_LAME_PRESET_V2:
	text = _("V2");
	break;
      case TAG_LAME_PRESET_V1:
	text = _("V1");
	break;
      case TAG_LAME_PRESET_V0:
	text = _("V0");
	break;
      case TAG_LAME_PRESET_R3MIX:
	text = _("r3mix");
	break;
      case TAG_LAME_PRESET_STANDARD:
	text = _("standard");
	break;
      case TAG_LAME_PRESET_EXTREME:
	text = _("extreme");
	break;
      case TAG_LAME_PRESET_INSANE:
	text = _("insane");
	break;
      case TAG_LAME_PRESET_STANDARD_FAST:
	text = _("standard/fast");
	break;
      case TAG_LAME_PRESET_EXTREME_FAST:
	text = _("extreme/fast");
	break;
      case TAG_LAME_PRESET_MEDIUM:
	text = _("medium");
	break;
      case TAG_LAME_PRESET_MEDIUM_FAST:
	text = _("medium/fast");
	break;
      }
      detail(_("Preset"), "%s", text ? text : _("unknown"));
    }

    detail(_("Unwise Settings"), "%s",
	   (tag->lame.flags & TAG_LAME_UNWISE) ? _("yes") : _("no"));

    detail(_("Encoding Flags"), "%s%s%s",
	   (tag->lame.flags & TAG_LAME_NSPSYTUNE)   ? "--nspsytune "   : "",
	   (tag->lame.flags & TAG_LAME_NSSAFEJOINT) ? "--nssafejoint " : "",
	   (tag->lame.flags & (TAG_LAME_NOGAP_NEXT | TAG_LAME_NOGAP_PREV)) ?
	   "--nogap" : "");

    text = 0;
    switch (tag->lame.flags & (TAG_LAME_NOGAP_NEXT | TAG_LAME_NOGAP_PREV)) {
    case TAG_LAME_NOGAP_NEXT:
      text = _("following");
      break;
    case TAG_LAME_NOGAP_PREV:
      text = _("preceding");
      break;
    case TAG_LAME_NOGAP_NEXT | TAG_LAME_NOGAP_PREV:
      text = _("following or preceding");
      break;
    }
    if (text)
      detail(_("No Gap"), "%s", text);

    text = _("Lowpass Filter");
    if (tag->lame.lowpass_filter == 0)
      detail(text, "%s", _("unknown"));
    else
      detail(text, _("%u Hz"), tag->lame.lowpass_filter);

    detail(_("ATH Type"), "%u", tag->lame.ath_type);

    detail(_("Noise Shaping"), "%u", tag->lame.noise_shaping);

    switch (tag->lame.surround) {
    case TAG_LAME_SURROUND_NONE:
      text = _("none");
      break;
    case TAG_LAME_SURROUND_DPL:
      text = _("DPL");
      break;
    case TAG_LAME_SURROUND_DPL2:
      text = _("DPL2");
      break;
    case TAG_LAME_SURROUND_AMBISONIC:
      text = _("Ambisonic");
      break;
    default:
      text = _("unknown");
    }
    detail(_("Surround"), "%s", text);

    detail(_("Start Delay"), _("%u samples"), tag->lame.start_delay);
    detail(_("End Padding"), _("%u samples"), tag->lame.end_padding);

    text = 0;
    switch (tag->lame.source_samplerate) {
    case TAG_LAME_SOURCE_32LOWER:
      text = _("32 kHz or lower");
      break;
    case TAG_LAME_SOURCE_44_1:
      text = _("44.1 kHz");
      break;
    case TAG_LAME_SOURCE_48:
      text = _("48 kHz");
      break;
    case TAG_LAME_SOURCE_HIGHER48:
      text = _("higher than 48 kHz");
      break;
    }
    if (text)
      detail(_("Source Rate"), "%s", text);

    if (tag->lame.gain != 0)
      detail(_("Gain"), _("%+.1f dB"), tag->lame.gain * 1.5);

    /* Replay Gain */

    if (tag->lame.peak > 0) {
      double peak = mad_f_todouble(tag->lame.peak);

      detail(_("Peak Amplitude"), _("%.8f (%+.1f dB)"),
	     peak, 20 * log10(peak));
    }

    if (tag->lame.replay_gain[0].name == RGAIN_NAME_RADIO)
      show_rgain(&tag->lame.replay_gain[0]);
    if (tag->lame.replay_gain[1].name == RGAIN_NAME_AUDIOPHILE)
      show_rgain(&tag->lame.replay_gain[1]);

    detail(_("Music Length"), _("%lu bytes"), tag->lame.music_length);
# if 0
    detail(_("Music CRC"), "0x%04x", tag->lame.music_crc);
# endif
  }

  if (tag->flags & TAG_XING) {
    if (tag->xing.flags & TAG_XING_FRAMES)
      detail(_("Audio Frames"), "%lu", tag->xing.frames);

    if ((tag->xing.flags & TAG_XING_BYTES) &&
	(!(tag->flags & TAG_LAME) ||
	 tag->lame.music_length != tag->xing.bytes))
      detail(_("Data Bytes"), "%lu", tag->xing.bytes);

    if ((tag->flags & TAG_VBR) && (tag->xing.flags & TAG_XING_SCALE))
      detail(_("VBR Scale"), _("%ld/100"), 100 - tag->xing.scale);
  }
}

enum {
  GAIN_VOLADJ   = 0x0001,
  GAIN_ATTAMP   = 0x0002,
  GAIN_RELATIVE = 0x0010
};

/*
 * NAME:	set_gain()
 * DESCRIPTION:	modify player gain information
 */
static
double set_gain(struct player *player, int how, double db)
{
  double *gain_db = 0;

  if (how & GAIN_ATTAMP)
    gain_db = &player->output.attamp_db;
  else if (how & GAIN_VOLADJ)
    gain_db = &player->output.voladj_db;

  if (gain_db) {
    if (how & GAIN_RELATIVE)
      *gain_db += db;
    else
      *gain_db  = db;
  }

  db = player->output.voladj_db + player->output.attamp_db;
  if (db > DB_MAX || db < DB_MIN) {
    db = (db > DB_MAX) ? DB_MAX : DB_MIN;
    player->output.attamp_db = db - player->output.voladj_db;
  }

  player->output.gain = db ? mad_f_tofixed(pow(10, db / 20)) : MAD_F_ONE;

  return db;
}

/*
 * NAME:	use_rgain()
 * DESCRIPTION:	select and employ a Replay Gain volume adjustment
 */
static
void use_rgain(struct player *player, struct rgain *list)
{
  struct rgain *rgain = &list[0];

  if ((player->output.replay_gain & PLAYER_RGAIN_AUDIOPHILE) &&
      list[1].name == RGAIN_NAME_AUDIOPHILE &&
      list[1].originator != RGAIN_ORIGINATOR_UNSPECIFIED)
    rgain = &list[1];

  if (RGAIN_VALID(rgain)) {
    double gain = RGAIN_DB(rgain);

    set_gain(player, GAIN_VOLADJ, gain);

    if (player->verbosity >= 0 ||
	(player->options & PLAYER_OPTION_SHOWTAGSONLY)) {
      char const *source;

      source = rgain_originator(rgain);
      assert(source);

      detail(_("Replay Gain"), _("%+.1f dB %s adjustment (%s)"),
	     gain, (rgain->name == RGAIN_NAME_RADIO) ?
	     _("radio") : _("audiophile"), source);
    }

    player->output.replay_gain |= PLAYER_RGAIN_SET;
  }
}

/*
 * NAME:	decode->filter()
 * DESCRIPTION:	perform filtering on decoded frame
 */
static
enum mad_flow decode_filter(void *data, struct mad_stream const *stream,
			    struct mad_frame *frame)
{
  struct player *player = data;

  /* output ancillary data */

  if (player->ancillary.file && stream->anc_bitlen &&
      write_ancillary(&player->ancillary,
		      stream->anc_ptr, stream->anc_bitlen) == -1)
    return MAD_FLOW_BREAK;

  /* first frame accounting */

  if (player->stats.absolute_framecount == 0) {
    if (player->input.tag.flags == 0 &&
	tag_parse(&player->input.tag, stream) == 0) {
      struct tag *tag = &player->input.tag;
      unsigned int frame_size;

      if (player->options & PLAYER_OPTION_SHOWTAGSONLY) {
	if (player->verbosity > 0)
	  show_tag(tag);
      }
      else {
	if ((tag->flags & TAG_LAME) &&
	    (player->output.replay_gain & PLAYER_RGAIN_ENABLED) &&
	    !(player->output.replay_gain & PLAYER_RGAIN_SET))
	  use_rgain(player, tag->lame.replay_gain);
      }

      if ((tag->flags & TAG_XING) &&
	  (tag->xing.flags & TAG_XING_FRAMES)) {
	player->stats.total_time = frame->header.duration;
	mad_timer_multiply(&player->stats.total_time, tag->xing.frames);
      }

      /* total stream byte size adjustment */

      frame_size = stream->next_frame - stream->this_frame;

      if (player->stats.total_bytes == 0) {
	if ((tag->flags & TAG_XING) && (tag->xing.flags & TAG_XING_BYTES) &&
	    tag->xing.bytes > frame_size)
	  player->stats.total_bytes = tag->xing.bytes - frame_size;
      }
      else if (player->stats.total_bytes >=
	       stream->next_frame - stream->this_frame)
	player->stats.total_bytes -= frame_size;

      return (player->options & PLAYER_OPTION_SHOWTAGSONLY) ?
	MAD_FLOW_STOP : MAD_FLOW_IGNORE;
    }
    else if (player->options & PLAYER_OPTION_SHOWTAGSONLY)
      return MAD_FLOW_STOP;

    ++player->stats.absolute_framecount;
    mad_timer_add(&player->stats.absolute_timer, frame->header.duration);

    ++player->stats.global_framecount;
    mad_timer_add(&player->stats.global_timer, frame->header.duration);

    if ((player->options & PLAYER_OPTION_SKIP) &&
	mad_timer_compare(player->stats.global_timer,
			  player->global_start) < 0)
      return MAD_FLOW_IGNORE;
  }

  /* run the filter chain */

  return filter_run(player->output.filters, frame);
}

/*
 * NAME:	process_id3()
 * DESCRIPTION:	display and process ID3 tag information
 */
static
void process_id3(struct id3_tag const *tag, struct player *player)
{
  struct id3_frame const *frame;

  /* display the tag */

  if (player->verbosity >= 0 || (player->options & PLAYER_OPTION_SHOWTAGSONLY))
    show_id3(tag);

  /*
   * The following is based on information from the
   * ID3 tag version 2.4.0 Native Frames informal standard.
   */

  /* length information */

  frame = id3_tag_findframe(tag, "TLEN", 0);
  if (frame) {
    union id3_field const *field;
    unsigned int nstrings;

    field    = id3_frame_field(frame, 1);
    nstrings = id3_field_getnstrings(field);

    if (nstrings > 0) {
      id3_latin1_t *latin1;

      latin1 = id3_ucs4_latin1duplicate(id3_field_getstrings(field, 0));
      if (latin1) {
	signed long ms;

	/*
	 * "The 'Length' frame contains the length of the audio file
	 * in milliseconds, represented as a numeric string."
	 */

	ms = atol(latin1);
	if (ms > 0)
	  mad_timer_set(&player->stats.total_time, 0, ms, 1000);

	free(latin1);
      }
    }
  }

  /* relative volume adjustment information */

  if ((player->options & PLAYER_OPTION_SHOWTAGSONLY) ||
      !(player->options & PLAYER_OPTION_IGNOREVOLADJ)) {
    frame = id3_tag_findframe(tag, "RVA2", 0);
    if (frame) {
      id3_latin1_t const *id;
      id3_byte_t const *data;
      id3_length_t length;

      enum {
	CHANNEL_OTHER         = 0x00,
	CHANNEL_MASTER_VOLUME = 0x01,
	CHANNEL_FRONT_RIGHT   = 0x02,
	CHANNEL_FRONT_LEFT    = 0x03,
	CHANNEL_BACK_RIGHT    = 0x04,
	CHANNEL_BACK_LEFT     = 0x05,
	CHANNEL_FRONT_CENTRE  = 0x06,
	CHANNEL_BACK_CENTRE   = 0x07,
	CHANNEL_SUBWOOFER     = 0x08
      };

      id   = id3_field_getlatin1(id3_frame_field(frame, 0));
      data = id3_field_getbinarydata(id3_frame_field(frame, 1), &length);

      assert(id && data);

      /*
       * "The 'identification' string is used to identify the situation
       * and/or device where this adjustment should apply. The following is
       * then repeated for every channel
       *
       *   Type of channel         $xx
       *   Volume adjustment       $xx xx
       *   Bits representing peak  $xx
       *   Peak volume             $xx (xx ...)"
       */

      while (length >= 4) {
	unsigned int peak_bytes;

	peak_bytes = (data[3] + 7) / 8;
	if (4 + peak_bytes > length)
	  break;

	if (data[0] == CHANNEL_MASTER_VOLUME) {
	  signed int voladj_fixed;
	  double voladj_float;

	  /*
	   * "The volume adjustment is encoded as a fixed point decibel
	   * value, 16 bit signed integer representing (adjustment*512),
	   * giving +/- 64 dB with a precision of 0.001953125 dB."
	   */

	  voladj_fixed  = (data[1] << 8) | (data[2] << 0);
	  voladj_fixed |= -(voladj_fixed & 0x8000);

	  voladj_float  = (double) voladj_fixed / 512;

	  set_gain(player, GAIN_VOLADJ, voladj_float);

	  if (player->verbosity >= 0) {
	    detail(_("Relative Volume"),
		   _("%+.1f dB adjustment (%s)"), voladj_float, id);
	  }

	  break;
	}

	data   += 4 + peak_bytes;
	length -= 4 + peak_bytes;
      }
    }
  }

  /* Replay Gain */

  if ((player->options & PLAYER_OPTION_SHOWTAGSONLY) ||
      ((player->output.replay_gain & PLAYER_RGAIN_ENABLED) &&
       !(player->output.replay_gain & PLAYER_RGAIN_SET))) {
    frame = id3_tag_findframe(tag, "RGAD", 0);
    if (frame) {
      id3_byte_t const *data;
      id3_length_t length;

      data = id3_field_getbinarydata(id3_frame_field(frame, 0), &length);
      assert(data);

      /*
       * Peak Amplitude                          $xx $xx $xx $xx
       * Radio Replay Gain Adjustment            $xx $xx
       * Audiophile Replay Gain Adjustment       $xx $xx
       */

      if (length >= 8) {
	struct mad_bitptr ptr;
	mad_fixed_t peak;
	struct rgain rgain[2];

	mad_bit_init(&ptr, data);

	peak = mad_bit_read(&ptr, 32) << 5;

	rgain_parse(&rgain[0], &ptr);
	rgain_parse(&rgain[1], &ptr);

	use_rgain(player, rgain);

	mad_bit_finish(&ptr);
      }
    }
  }
}

/*
 * NAME:	show_status()
 * DESCRIPTION: generate and output stream statistics
 */
static
void show_status(struct stats *stats,
		 struct mad_header const *header, char const *label, int now)
{
  signed long seconds;
  static char const *const layer_str[3] = { N_("I"), N_("II"), N_("III") };
  static char const *const mode_str[4] = {
    N_("single channel"), N_("dual channel"), N_("joint stereo"), N_("stereo")
  };

  if (header) {
    unsigned int bitrate;

    bitrate = header->bitrate / 1000;

    stats->vbr_rate += bitrate;
    stats->vbr_frames++;

    stats->vbr += (stats->bitrate && stats->bitrate != bitrate) ? 128 : -1;
    if (stats->vbr < 0)
      stats->vbr = 0;
    else if (stats->vbr > 512)
      stats->vbr = 512;

    stats->bitrate = bitrate;
  }

  seconds = mad_timer_count(stats->global_timer, MAD_UNITS_SECONDS);
  if (seconds != stats->nsecs || !on_same_line || now) {
    mad_timer_t timer;
    char time_str[18];
    char const *joint_str = "";

    stats->nsecs = seconds;

    switch (stats->show) {
    case STATS_SHOW_OVERALL:
      timer = stats->global_timer;
      break;

    case STATS_SHOW_CURRENT:
      timer = stats->absolute_timer;
      break;

    case STATS_SHOW_REMAINING:
      timer = stats->total_time;

      if (mad_timer_sign(timer) == 0 && stats->total_bytes) {
	unsigned long rate;

	/* estimate based on size and bitrate */

	rate = stats->vbr ?
	  stats->vbr_rate * 125 / stats->vbr_frames : stats->bitrate * 125UL;

	mad_timer_set(&timer, 0, stats->total_bytes, rate);
      }

      mad_timer_negate(&timer);
      mad_timer_add(&timer, stats->absolute_timer);
      break;
    }

    mad_timer_string(timer, time_str, " %02lu:%02u:%02u",
		     MAD_UNITS_HOURS, 0, 0);
    if (mad_timer_sign(timer) < 0)
      time_str[0] = '-';

    if (label || stats->label) {
      message("%s %s", time_str, label ? label : stats->label);
      stats->label = now ? label : 0;
    }
    else if (header) {
      if (header->mode == MAD_MODE_JOINT_STEREO) {
	switch (header->flags & (MAD_FLAG_MS_STEREO | MAD_FLAG_I_STEREO)) {
	case 0:
	  joint_str = _(" (LR)");
	  break;

	case MAD_FLAG_MS_STEREO:
	  joint_str = _(" (MS)");
	  break;

	case MAD_FLAG_I_STEREO:
	  joint_str = _(" (I)");
	  break;

	case (MAD_FLAG_MS_STEREO | MAD_FLAG_I_STEREO):
	  joint_str = _(" (MS+I)");
	  break;
	}
      }

      message(_("%s Layer %s, %s%u kbps%s, %u Hz, %s%s, %s"),
	      time_str, gettext(layer_str[header->layer - 1]),
	      stats->vbr ? _("VBR (avg ") : "",
	      stats->vbr ? ((stats->vbr_rate * 2) /
			    stats->vbr_frames + 1) / 2 : stats->bitrate,
	      stats->vbr ? _(")") : "",
	      header->samplerate, gettext(mode_str[header->mode]), joint_str,
	      (header->flags & MAD_FLAG_PROTECTION) ? _("CRC") : _("no CRC"));
    }
    else
      message("%s", time_str);
  }
}

/*
 * NAME:	decode->output()
 * DESCRIPTION: configure audio module and output decoded samples
 */
static
enum mad_flow decode_output(void *data, struct mad_header const *header,
			    struct mad_pcm *pcm)
{
  struct player *player = data;
  struct output *output = &player->output;
  mad_fixed_t const *ch1, *ch2;
  unsigned int nchannels;
  union audio_control control;

  ch1 = pcm->samples[0];
  ch2 = pcm->samples[1];

  switch (nchannels = pcm->channels) {
  case 1:
    ch2 = 0;
    if (output->select == PLAYER_CHANNEL_STEREO) {
      ch2 = ch1;
      nchannels = 2;
    }
    break;

  case 2:
    switch (output->select) {
    case PLAYER_CHANNEL_RIGHT:
      ch1 = ch2;
      /* fall through */

    case PLAYER_CHANNEL_LEFT:
      ch2 = 0;
      nchannels = 1;
      /* fall through */

    case PLAYER_CHANNEL_STEREO:
      break;

    default:
      if (header->mode == MAD_MODE_DUAL_CHANNEL) {
	if (output->select == PLAYER_CHANNEL_DEFAULT) {
	  if (player->verbosity >= -1) {
	    error("output",
		  _("no channel selected for dual channel; using first"));
	  }

	  output->select = -PLAYER_CHANNEL_LEFT;
	}

	ch2 = 0;
	nchannels = 1;
      }
    }
  }

  if (output->channels_in != nchannels ||
      output->speed_in != pcm->samplerate) {
    unsigned int speed_request;

    if (player->verbosity >= 1 &&
	pcm->samplerate != header->samplerate) {
      error("output", _("decoded sample frequency %u Hz"),
	    pcm->samplerate);
    }

    speed_request = output->speed_request ?
      output->speed_request : pcm->samplerate;

    audio_control_init(&control, AUDIO_COMMAND_CONFIG);

    control.config.channels  = nchannels;
    control.config.speed     = speed_request;
    control.config.precision = output->precision_in;

    if (output->command(&control) == -1) {
      error("output", audio_error);
      return MAD_FLOW_BREAK;
    }

    output->channels_in  = nchannels;
    output->speed_in     = pcm->samplerate;

    output->channels_out  = control.config.channels;
    output->speed_out     = control.config.speed;
    output->precision_out = control.config.precision;

    if (player->verbosity >= -1 &&
	output->channels_in != output->channels_out) {
      if (output->channels_in == 1)
	error("output", _("mono output not available; forcing stereo"));
      else {
	error("output", _("stereo output not available; using first channel "
			  "(use -m to mix)"));
      }
    }

    if (player->verbosity >= -1 &&
	output->precision_in &&
	output->precision_in != output->precision_out) {
      error("output", _("bit depth %u not available; using %u"),
	    output->precision_in, output->precision_out);
    }

    if (player->verbosity >= -1 &&
	speed_request != output->speed_out) {
      error("output", _("sample frequency %u Hz not available; using %u Hz"),
	    speed_request, output->speed_out);
    }

    /* check whether resampling is necessary */
    if (abs(output->speed_out - output->speed_in) <
	(long) FREQ_TOLERANCE * output->speed_in / 100) {
      if (output->resampled) {
	resample_finish(&output->resample[0]);
	resample_finish(&output->resample[1]);

	free(output->resampled);
	output->resampled = 0;
      }
    }
    else {
      if (output->resampled) {
	resample_finish(&output->resample[0]);
	resample_finish(&output->resample[1]);
      }
      else {
	output->resampled = malloc(sizeof(*output->resampled));
	if (output->resampled == 0) {
	  error("output",
		_("not enough memory to allocate resampling buffer"));

	  output->speed_in = 0;
	  return MAD_FLOW_BREAK;
	}
      }

      if (resample_init(&output->resample[0],
			output->speed_in, output->speed_out) == -1 ||
	  resample_init(&output->resample[1],
			output->speed_in, output->speed_out) == -1) {
	error("output", _("cannot resample %u Hz to %u Hz"),
	      output->speed_in, output->speed_out);

	free(output->resampled);
	output->resampled = 0;

	output->speed_in = 0;
	return MAD_FLOW_BREAK;
      }
      else if (player->verbosity >= -1) {
	error("output", _("resampling %u Hz to %u Hz"),
	      output->speed_in, output->speed_out);
      }
    }
  }

  audio_control_init(&control, AUDIO_COMMAND_PLAY);

  if (output->channels_in != output->channels_out)
    ch2 = (output->channels_out == 2) ? ch1 : 0;


  if (output->resampled) {
    control.play.nsamples = resample_block(&output->resample[0],
					   pcm->length, ch1,
					   (*output->resampled)[0]);
    control.play.samples[0] = (*output->resampled)[0];

    if (ch2 == ch1)
      control.play.samples[1] = control.play.samples[0];
    else if (ch2) {
      resample_block(&output->resample[1], pcm->length, ch2,
		     (*output->resampled)[1]);
      control.play.samples[1] = (*output->resampled)[1];
    }
    else
      control.play.samples[1] = 0;
  }
  else {
    control.play.nsamples   = pcm->length;
    control.play.samples[0] = ch1;
    control.play.samples[1] = ch2;
  }

  control.play.mode  = output->mode;
  control.play.stats = &player->stats.audio;

// printf ("player: executing output->command(&control) \n");
// printf ("player: %p\n", output);
// printf ("player: %p\n", output->command);
// printf ("player: %p\n", &control);

  if (output->command(&control) == -1) {
    error("output", audio_error);
    return MAD_FLOW_BREAK;
  }

// printf ("player: going well \n");

  ++player->stats.play_framecount;
  mad_timer_add(&player->stats.play_timer, header->duration);

  if (player->verbosity > 0)
    show_status(&player->stats, header, 0, 0);

  return MAD_FLOW_CONTINUE;
}

/*
 * NAME:	get_id3()
 * DESCRIPTION:	read and parse an ID3 tag from a stream
 */
static
struct id3_tag *get_id3(struct mad_stream *stream, id3_length_t tagsize,
			struct input *input)
{
  struct id3_tag *tag = 0;
  id3_length_t count;
  id3_byte_t const *data;
  id3_byte_t *allocated = 0;

  count = stream->bufend - stream->this_frame;

  if (tagsize <= count) {
    data = stream->this_frame;
    mad_stream_skip(stream, tagsize);
  }
  else {
    allocated = malloc(tagsize);
    if (allocated == 0) {
      error("id3", _("not enough memory to allocate tag data buffer"));
      goto fail;
    }

    memcpy(allocated, stream->this_frame, count);
    mad_stream_skip(stream, count);

    while (count < tagsize) {
      int len;

      do
	len = read(input->fd, allocated + count, tagsize - count);
      while (len == -1 && errno == EINTR);

      if (len == -1) {
	error("id3", ":read");
	goto fail;
      }

      if (len == 0) {
	error("id3", _("EOF while reading tag data"));
	goto fail;
      }

      count += len;
    }

    data = allocated;
  }

  tag = id3_tag_parse(data, tagsize);

 fail:
  if (allocated)
    free(allocated);

  return tag;
}

/*
 * NAME:	decode->error()
 * DESCRIPTION:	handle a decoding error
 */
static
enum mad_flow decode_error(void *data, struct mad_stream *stream,
			   struct mad_frame *frame)
{
  struct player *player = data;
  signed long tagsize;

  switch (stream->error) {
  case MAD_ERROR_BADDATAPTR:
    return MAD_FLOW_CONTINUE;

  case MAD_ERROR_LOSTSYNC:
    tagsize = id3_tag_query(stream->this_frame,
			    stream->bufend - stream->this_frame);
    if (tagsize > 0) {
      if (player->options & PLAYER_OPTION_STREAMID3) {
	struct id3_tag *tag;

	tag = get_id3(stream, tagsize, &player->input);
	if (tag) {
	  process_id3(tag, player);
	  id3_tag_delete(tag);
	}
      }
      else
	mad_stream_skip(stream, tagsize);

      if (player->stats.total_bytes >= tagsize)
	player->stats.total_bytes -= tagsize;

      return MAD_FLOW_CONTINUE;
    }

    /* fall through */

  default:
    if (player->verbosity >= -1 &&
	!(player->options & PLAYER_OPTION_SHOWTAGSONLY) &&
	((stream->error == MAD_ERROR_LOSTSYNC && !player->input.eof)
	 || stream->sync) &&
	player->stats.global_framecount != player->stats.error_frame) {
      error("error", _("frame %lu: %s"),
	    player->stats.absolute_framecount, mad_stream_errorstr(stream));
      player->stats.error_frame = player->stats.global_framecount;
    }
  }

  if (stream->error == MAD_ERROR_BADCRC) {
    if (player->stats.global_framecount == player->stats.mute_frame)
      mad_frame_mute(frame);

    player->stats.mute_frame = player->stats.global_framecount + 1;

    return MAD_FLOW_IGNORE;
  }

  return MAD_FLOW_CONTINUE;
}

/*
 * NAME:	decode()
 * DESCRIPTION:	decode and output audio for an open file
 */
static
int decode(struct player *player)
{
//  struct stat s;
  struct mad_decoder decoder;
  int options, result;

//  if (fstat(player->input.fd, &s) == -1) {
//    error("decode", ":fstat");
//    return -1;
//  }

//  if (S_ISREG(stat.st_mode))
//    player->stats.total_bytes = stat.st_size;

  tag_init(&player->input.tag);

  /* prepare input buffers */

# if defined(HAVE_MMAP)
  if (S_ISREG(s.st_mode) && s.st_size > 0) {
    player->input.length = s.st_size;

    player->input.fdm = map_file(player->input.fd, player->input.length);
    if (player->input.fdm == 0 && player->verbosity >= 0)
      error("decode", ":mmap");

    player->input.data = player->input.fdm;
  }
# endif

  if (player->input.data == 0) {
    player->input.data = malloc(MPEG_BUFSZ);
    if (player->input.data == 0) {
      error("decode", _("not enough memory to allocate input buffer"));
      return -1;
    }

    player->input.length = 0;
  }

  player->input.eof = 0;

  /* reset statistics */
  player->stats.absolute_timer        = mad_timer_zero;
  player->stats.play_timer            = mad_timer_zero;
  player->stats.absolute_framecount   = 0;
  player->stats.play_framecount       = 0;
  player->stats.error_frame           = -1;
  player->stats.vbr                   = 0;
  player->stats.bitrate               = 0;
  player->stats.vbr_frames            = 0;
  player->stats.vbr_rate              = 0;
  player->stats.audio.clipped_samples = 0;
  player->stats.audio.peak_clipping   = 0;
  player->stats.audio.peak_sample     = 0;

  mad_decoder_init(&decoder, player,
# if defined(HAVE_MMAP)
		   player->input.fdm ? decode_input_mmap :
# endif
		   decode_input_read,
		   decode_header, decode_filter,
		   player->output.command ? decode_output : 0,
		   decode_error, 0);

  options = 0;
  if (player->options & PLAYER_OPTION_DOWNSAMPLE)
    options |= MAD_OPTION_HALFSAMPLERATE;
  if (player->options & PLAYER_OPTION_IGNORECRC)
    options |= MAD_OPTION_IGNORECRC;

  mad_decoder_options(&decoder, options);

  result = mad_decoder_run(&decoder, MAD_DECODER_MODE_SYNC);

  mad_decoder_finish(&decoder);

# if defined(HAVE_MMAP)
  if (player->input.fdm) {
    if (unmap_file(player->input.fdm, player->input.length) == -1) {
      error("decode", ":munmap");
      result = -1;
    }

    player->input.fdm = 0;

    if (!player->input.eof)
      player->input.data = 0;
  }
# endif

  if (player->input.data) {
    free(player->input.data);
    player->input.data = 0;
  }

  tag_finish(&player->input.tag);

  return result;
}

extern int mediaMode;

/*
 * NAME:	play_one()
 * DESCRIPTION:	open and play a single file
 */
static
int play_one(struct player *player)
{
  char const *file = player->playlist.entries[player->playlist.current];
  int result;

#ifndef STDIN_FILENO
#define STDIN_FILENO 0
#endif

  if (strcmp(file, "-") == 0) {
    if (isatty(STDIN_FILENO)) {
      error(0, "%s: %s", _("stdin"), _("is a tty"));
      return -1;
    }

    player->input.path = _("stdin");
    player->input.fd   = STDIN_FILENO;
  }
  else {

    player->input.path = file;
//    player->input.fd   = open(file, O_RDONLY | O_BINARY);
    player->input.fd   = OpenFile(file, O_RDONLY, mediaMode);

	if (mediaMode == 4) // host
	{
		BstdFile = NewBstdFile(player->input.fd, mediaMode);
	}
	else
		BstdFile = NewBstdFile(player->input.fd, 0);
	if(BstdFile==NULL)
	{
		printf("could not open file\n");		
		return(1);
	}
	else
		printf("opened file\n");		

    if (player->input.fd == -1) {
      error(0, ":", file);
      return -1;
    }
  }

  if (player->verbosity >= 0 &&
      player->playlist.length > 1)
    message(">> %s\n", player->input.path);

  /* reset file information */

  player->stats.total_bytes = 0;
  player->stats.total_time  = mad_timer_zero;

  if (!(player->options & PLAYER_OPTION_IGNOREVOLADJ))
    set_gain(player, GAIN_VOLADJ, 0);

  player->output.replay_gain &= ~PLAYER_RGAIN_SET;

  /* try reading ID3 tag information now (else read later from stream) */
  {
    int fd;
    struct id3_file *file;

    player->options &= ~PLAYER_OPTION_STREAMID3;

//    fd = dup(player->input.fd);
    fd = player->input.fd;
    file = id3_file_fdopen(fd, ID3_FILE_MODE_READONLY);
    if (file == 0) {
//      close(fd);
      CloseFile(fd, mediaMode);
      player->options |= PLAYER_OPTION_STREAMID3;
    }
    else {
      process_id3(id3_file_tag(file), player);
      id3_file_close(file);
    }
  }

  result = decode(player);

  if (result == 0 && player->verbosity >= 0 &&
      !(player->options & PLAYER_OPTION_SHOWTAGSONLY)) {
    char time_str[19], db_str[7];
    char const *peak_str;
    mad_fixed_t peak;

    mad_timer_string(player->stats.play_timer, time_str, "%lu:%02u:%02u.%1u",
		     MAD_UNITS_HOURS, MAD_UNITS_DECISECONDS, 0);

# if defined(HAVE_LOCALECONV)
    {
      char *point;

      point = strchr(time_str, '.');
      if (point)
	*point = *localeconv()->decimal_point;
    }
# endif

    peak = MAD_F_ONE + player->stats.audio.peak_clipping;
    if (peak == MAD_F_ONE)
      peak = player->stats.audio.peak_sample;

    if (peak == 0)
      peak_str = "-inf";
    else {
      sprintf(db_str, "%+.1f", 20 * log10(mad_f_todouble(peak)));
      peak_str = db_str;
    }

    message("%lu %s (%s), %s dB %s, %lu %s\n",
	    player->stats.play_framecount, player->stats.play_framecount == 1 ?
	    _("frame decoded") : _("frames decoded"), time_str,
	    peak_str, _("peak amplitude"), player->stats.audio.clipped_samples,
	    player->stats.audio.clipped_samples == 1 ?
	    _("clipped sample") : _("clipped samples"));
  }

  if (player->input.fd != STDIN_FILENO &&
      close(player->input.fd) == -1 && result == 0) {
    error(0, ":", player->input.path);
    result = -1;
  }

  return result;
}

/*
 * NAME:	play_all()
 * DESCRIPTION:	run the player's playlist
 */
static
int play_all(struct player *player)
{
  int count, i, j, result = 0;
  struct playlist *playlist = &player->playlist;
  char const *tmp;

  /* set up playlist */

  count = playlist->length;

  if (player->options & PLAYER_OPTION_SHUFFLE) {
//    srand(time(0));
    srand(0);

    /* initial shuffle */
    for (i = 0; i < count; ++i) {
      j = rand() % count;

      tmp = playlist->entries[i];
      playlist->entries[i] = playlist->entries[j];
      playlist->entries[j] = tmp;
    }
  }

  /* run playlist */

  while (count && (player->repeat == -1 || player->repeat--)) {
    while (playlist->current < playlist->length) {
      i = playlist->current;

      if (playlist->entries[i] == 0) {
	++playlist->current;
	continue;
      }

      player->control = PLAYER_CONTROL_DEFAULT;

      if (play_one(player) == -1) {
	playlist->entries[i] = 0;
	--count;

	result = -1;
      }

      if ((player->options & PLAYER_OPTION_TIMED) &&
	  mad_timer_compare(player->stats.global_timer,
			    player->global_stop) > 0) {
	count = 0;
	break;
      }

      switch (player->control) {
      case PLAYER_CONTROL_DEFAULT:
	if ((player->options & PLAYER_OPTION_SHUFFLE) && player->repeat &&
	    ++i < playlist->length) {
	  /* pick something from the next half only */
	  j = (i + rand() % ((playlist->length + 1) / 2)) % playlist->length;

	  tmp = playlist->entries[i];
	  playlist->entries[i] = playlist->entries[j];
	  playlist->entries[j] = tmp;
	}
	/* fall through */

      case PLAYER_CONTROL_NEXT:
	++playlist->current;
	break;

      case PLAYER_CONTROL_PREVIOUS:
	do {
	  if (playlist->current-- == 0)
	    playlist->current = playlist->length;
	}
	while (playlist->current < playlist->length &&
	       playlist->entries[playlist->current] == 0);
	break;

      case PLAYER_CONTROL_REPLAY:
	break;

      case PLAYER_CONTROL_STOP:
	playlist->current = playlist->length;
	count = 0;
	break;
      }
    }

    playlist->current = 0;
  }

  return result;
}

/*
 * NAME:	stop_audio()
 * DESCRIPTION:	stop playing the audio device immediately
 */
static
int stop_audio(struct player *player, int flush)
{
  int result = 0;

  if (player->output.command) {
    union audio_control control;

    audio_control_init(&control, AUDIO_COMMAND_STOP);
    control.stop.flush = flush;

    result = player->output.command(&control);
  }

  return result;
}

# if defined(USE_TTY)
/*
 * NAME:	readkey()
 * DESCRIPTION:	read a keypress from the keyboard
 */
static
int readkey(int blocking)
{
# if !defined(_WIN32)
  unsigned char key;
  ssize_t count;

  if (!blocking) {
    /* tty_fd should be a tty in noncanonical mode with VMIN = VTIME = 0 */

    count = read(tty_fd, &key, 1);
    if (count == -1 && errno != EINTR) {
      error("tty", ":read");
      return -1;
    }

    return (count == 1) ? key : 0;
  }
  else {
    struct termios tty, save_tty;

    if (tcgetattr(tty_fd, &tty) == -1) {
      error("tty", ":tcgetattr");
      return -1;
    }

    save_tty = tty;

    /* change terminal temporarily to get a blocking read() */

    tty.c_cc[VMIN] = 1;

    if (tcsetattr(tty_fd, TCSANOW, &tty) == -1) {
      error("tty", ":tcsetattr");
      return -1;
    }

    do
      count = read(tty_fd, &key, 1);
    while (count == -1 && errno == EINTR);

    if (count == -1)
      error("tty", ":read");

    if (tcsetattr(tty_fd, TCSANOW, &save_tty) == -1) {
      error("tty", ":tcsetattr");
      return -1;
    }

    if (count == -1)
      return -1;

    return (count == 1) ? key : 0;
  }
# elif defined(_WIN32)
  HANDLE console;
  INPUT_RECORD input;
  DWORD count;

  console = GetStdHandle(STD_INPUT_HANDLE);

  do {
    if (GetNumberOfConsoleInputEvents(console, &count) == 0) {
      error("tty", "GetNumberOfConsoleInputEvents() failed");
      return -1;
    }

    if (count == 0) {
      if (!blocking)
	return 0;
      else {
	/* this is necessary to keep Windows from hanging (!) */
	Sleep(500);

	switch (WaitForSingleObject(console, INFINITE)) {
	case WAIT_ABANDONED:
	case WAIT_OBJECT_0:
	  continue;

	case WAIT_TIMEOUT:
	default:
	  /* ? */
	case WAIT_FAILED:
	  error("tty", "WaitForSingleObject() failed");
	  return -1;
	}
      }
    }

    if (ReadConsoleInput(console, &input, 1, &count) == 0 || count != 1) {
      error("tty", "ReadConsoleInput() failed");
      return -1;
    }
  }
  while (input.EventType != KEY_EVENT || !input.Event.KeyEvent.bKeyDown ||
	 input.Event.KeyEvent.uChar.AsciiChar == 0);

  return (unsigned char) input.Event.KeyEvent.uChar.AsciiChar;
# endif

  return blocking ? -1 : 0;
}

/*
 * NAME:	tty_filter()
 * DESCRIPTION:	process TTY commands
 */
static
enum mad_flow tty_filter(void *data, struct mad_frame *frame)
{
  struct player *player = data;
  enum mad_flow flow = MAD_FLOW_CONTINUE;
  int command, stopped = 0;

  command = readkey(0);
  if (command == -1)
    return MAD_FLOW_BREAK;

 again:
  switch (command) {
  case KEY_STOP:
    stopped = 1;

    player->control = PLAYER_CONTROL_REPLAY;
    flow = MAD_FLOW_STOP;

    /* fall through */

  case KEY_PAUSE:
    stop_audio(player, stopped);
    message(" --%s--", stopped ? _("Stopped") : _("Paused"));

    command = readkey(1);

    message("");

    if (command == -1)
      return MAD_FLOW_BREAK;

    if (command != KEY_PAUSE)
      goto again;

    break;

  case KEY_FORWARD:
  case KEY_CTRL('n'):
  case '>':
    player->control = PLAYER_CONTROL_NEXT;
    goto stop;

  case KEY_BACK:
  case KEY_CTRL('p'):
  case '<':
    {
      mad_timer_t threshold;

      mad_timer_set(&threshold, 4, 0, 0);

      player->control =
	(stopped ||
	 mad_timer_compare(player->stats.play_timer, threshold) < 0) ?
	PLAYER_CONTROL_PREVIOUS : PLAYER_CONTROL_REPLAY;
    }
    goto stop;

  case KEY_QUIT:
  case KEY_CTRL('c'):
  case 'Q':
    player->control = PLAYER_CONTROL_STOP;
    goto stop;

  case KEY_INFO:
  case '?':
    if (player->verbosity <= 0) {
      show_status(&player->stats, 0, player->input.path, 1);
      message("\n");
    }
    break;

  case KEY_TIME:
    if (player->verbosity > 0) {
      char const *label = 0;

      switch (player->stats.show) {
      case STATS_SHOW_OVERALL:
	player->stats.show = STATS_SHOW_REMAINING;
	label = N_("[Current Time Remaining]");
	break;

      case STATS_SHOW_REMAINING:
	player->stats.show = STATS_SHOW_CURRENT;
	label = N_("[Current Time]");
	break;

      case STATS_SHOW_CURRENT:
	player->stats.show = STATS_SHOW_OVERALL;
	label = N_("[Overall Time]");
	break;
      }

      show_status(&player->stats, 0, gettext(label), 1);
    }
    break;

  case KEY_GAINDECR:
  case KEY_GAININCR:
  case KEY_GAINZERO:
  case KEY_GAININFO:
    {
      double db;

      switch (command) {
      case KEY_GAINDECR:
	db = set_gain(player, GAIN_ATTAMP | GAIN_RELATIVE, -0.5);
	break;

      case KEY_GAININCR:
	db = set_gain(player, GAIN_ATTAMP | GAIN_RELATIVE, +0.5);
	break;

      case KEY_GAINZERO:
	db = set_gain(player, GAIN_ATTAMP, 0);
	break;

      default:
	db = set_gain(player, 0, 0);
	break;
      }

      if (player->verbosity > 0) {
	static char status[15];

	sprintf(status, "%+.1f dB gain", db);
	show_status(&player->stats, 0, status, 1);
      }
    }
    break;
  }

  return flow;

 stop:
  stop_audio(player, 1);
  return MAD_FLOW_STOP;
}
# endif

/*
 * NAME:	addfilter()
 * DESCRIPTION:	insert a filter at the beginning of the filter chain
 */
static
int addfilter(struct player *player, filter_func_t *func, void *data)
{
  struct filter *filter;

  filter = filter_new(func, data, player->output.filters);
  if (filter == 0)
    return -1;

  player->output.filters = filter;

  return 0;
}

/*
 * NAME:	setup_filters()
 * DESCRIPTION:	create output filters
 */
static
int setup_filters(struct player *player)
{
  /* filters must be added in reverse order */

# if defined(EXPERIMENTAL)
  if ((player->options & PLAYER_OPTION_EXTERNALMIX) &&
      addfilter(player, mixer_filter, stdout) == -1)
    return -1;

  if ((player->options & PLAYER_OPTION_EXPERIMENTAL) &&
      addfilter(player, experimental_filter, 0) == -1)
    return -1;
# endif

  if ((player->options & PLAYER_OPTION_FADEIN) &&
      addfilter(player, fadein_filter, player) == -1)
    return -1;

  addfilter(player, gain_filter, &player->output.gain);

  if (player->output.select == PLAYER_CHANNEL_MONO &&
      addfilter(player, mono_filter, player) == -1)
    return -1;

# if defined(USE_TTY)
  if ((player->options & PLAYER_OPTION_TTYCONTROL) &&
      addfilter(player, tty_filter, player) == -1)
    return -1;
# endif

  return 0;
}

# if defined(USE_TTY) && !defined(_WIN32)
/*
 * NAME:	restore_tty()
 * DESCRIPTION:	revert to previous terminal settings
 */
static
int restore_tty(int interrupt)
{
  struct termios tty;
  struct sigaction action;
  int result = 0;

  if (tcgetattr(tty_fd, &tty) == 0 &&
      tcsetattr(tty_fd, interrupt ? TCSAFLUSH : TCSADRAIN,
		&save_tty) == -1) {
    if (!interrupt)
      error("tty", ":tcsetattr");
    result = -1;
  }

  save_tty = tty;

  if (sigaction(SIGINT, 0, &action) == 0 &&
      sigaction(SIGINT, &save_sigint, 0) == -1) {
    if (!interrupt)
      error("tty", ":sigaction(SIGINT)");
    result = -1;
  }

  save_sigint = action;

  if (sigaction(SIGTSTP, 0, &action) == 0 &&
      sigaction(SIGTSTP, &save_sigtstp, 0) == -1) {
    if (!interrupt)
      error("tty", ":sigaction(SIGTSTP)");
    result = -1;
  }

  save_sigtstp = action;

  if (!interrupt) {
    if (close(tty_fd) == -1) {
      error("tty", ":close");
      result = -1;
    }

    tty_fd = -1;
  }

  return result;
}

/*
 * NAME:	signal_handler()
 * DESCRIPTION:	restore tty state after software interrupt
 */
static
void signal_handler(int signal)
{
  static struct sigaction save_sigcont;

  /* restore tty state and previous signal actions */

  restore_tty(1);

  /* handle SIGCONT after SIGTSTP */

  switch (signal) {
  case SIGTSTP:
    {
      struct sigaction action;

      sigaction(SIGCONT, 0, &save_sigcont);

      action = save_sigcont;
      action.sa_handler = signal_handler;
      sigemptyset(&action.sa_mask);
      sigaddset(&action.sa_mask, SIGTSTP);
      sigaddset(&action.sa_mask, SIGINT);
      action.sa_flags = 0;

      sigaction(SIGCONT, &action, 0);
    }
    break;

  case SIGCONT:
    sigaction(SIGCONT, &save_sigcont, 0);
    on_same_line = 0;  /* redraw status line */
    break;
  }

  /* re-send signal, which is currently blocked */

  kill(getpid(), signal);

  /* return to previous thread, which should immediately receive the signal */

  return;
}

/*
 * NAME:	setup_tty()
 * DESCRIPTION:	change terminal parameters and signal handlers
 */
static
int setup_tty(void)
{
  struct termios tty;
  struct sigaction action;

  /* open controlling terminal */

  tty_fd = open(TTY_DEVICE, O_RDONLY);
  if (tty_fd == -1) {
    error("tty", ":", TTY_DEVICE);
    return -1;
  }

  /* save current terminal and signal settings */

  if (tcgetattr(tty_fd, &save_tty) == -1) {
    error("tty", ":tcgetattr");
    return -1;
  }

  if (sigaction(SIGTSTP, 0, &save_sigtstp) == -1) {
    error("tty", ":sigaction(SIGTSTP)");
    return -1;
  }

  if (sigaction(SIGINT, 0, &save_sigint) == -1) {
    error("tty", ":sigaction(SIGINT)");
    return -1;
  }

  /* catch SIGTSTP and SIGINT so the tty state can be restored */

  action = save_sigtstp;
  action.sa_handler = signal_handler;
  sigemptyset(&action.sa_mask);
  sigaddset(&action.sa_mask, SIGINT);
# if 0  /* on some systems (Mac OS X) this remains masked upon continue (?!) */
  sigaddset(&action.sa_mask, SIGCONT);
# endif
  action.sa_flags = 0;

  if (sigaction(SIGTSTP, &action, 0) == -1) {
    error("tty", ":sigaction(SIGTSTP)");
    goto fail;
  }

  action = save_sigint;
  action.sa_handler = signal_handler;
  sigemptyset(&action.sa_mask);
  sigaddset(&action.sa_mask, SIGTSTP);
  sigaddset(&action.sa_mask, SIGCONT);
  action.sa_flags = 0;

  if (sigaction(SIGINT, &action, 0) == -1) {
    error("tty", ":sigaction(SIGINT)");
    goto fail;
  }

  /* turn off echo and canonical mode */

  tty = save_tty;

  tty.c_lflag &= ~(ECHO | ICANON);

  /* set VMIN = VTIME = 0 so read() always returns immediately */

  tty.c_cc[VMIN]  = 0;
  tty.c_cc[VTIME] = 0;

  if (tcsetattr(tty_fd, TCSAFLUSH, &tty) == -1) {
    error("tty", ":tcsetattr");
    goto fail;
  }

  return 0;

 fail:
  sigaction(SIGINT,  &save_sigint,  0);
  sigaction(SIGTSTP, &save_sigtstp, 0);
  return -1;
}
# endif

/*
 * NAME:	silence()
 * DESCRIPTION:	output silence for a period of time
 */
static
int silence(struct player *player, mad_timer_t duration, char const *label)
{
  union audio_control control;
  unsigned int nchannels, speed, nsamples;
  mad_fixed_t *samples;
  mad_timer_t unit;
  int result = 0;

  audio_control_init(&control, AUDIO_COMMAND_CONFIG);
  control.config.channels = 2;
  control.config.speed    = 44100;

  if (player->output.command(&control) == -1) {
    error("audio", audio_error);
    return -1;
  }

  nchannels = control.config.channels;
  speed     = control.config.speed;
  nsamples  = speed > MAX_NSAMPLES ? MAX_NSAMPLES : speed;

  player->output.channels_in  = nchannels;
  player->output.channels_out = nchannels;
  player->output.speed_in     = speed;
  player->output.speed_out    = speed;

  samples = calloc(nsamples, sizeof(mad_fixed_t));
  if (samples == 0) {
    error("silence", _("not enough memory to allocate sample buffer"));
    return -1;
  }

  audio_control_init(&control, AUDIO_COMMAND_PLAY);
  control.play.nsamples   = nsamples;
  control.play.samples[0] = samples;
  control.play.samples[1] = (nchannels == 2) ? samples : 0;
  control.play.mode       = player->output.mode;
  control.play.stats      = &player->stats.audio;

  mad_timer_set(&unit, 0, nsamples, speed);

  for (mad_timer_negate(&duration);
       mad_timer_sign(duration) < 0;
       mad_timer_add(&duration, unit)) {
    if (mad_timer_compare(unit, mad_timer_abs(duration)) > 0) {
      unit = mad_timer_abs(duration);
      control.play.nsamples = mad_timer_fraction(unit, speed);
    }

# if defined(USE_TTY)
    if ((player->options & PLAYER_OPTION_TTYCONTROL) &&
	tty_filter(player, 0) != MAD_FLOW_CONTINUE)
      goto fail;
# endif

    if (player->output.command(&control) == -1) {
      error("audio", audio_error);
      goto fail;
    }

    mad_timer_add(&player->stats.global_timer, unit);

    if (player->verbosity > 0)
      show_status(&player->stats, 0, label, 0);
  }

  if (0) {
  fail:
    result = -1;
  }

  free(samples);

  return result;
}

/*
 * NAME:	player->run()
 * DESCRIPTION:	begin playback
 */
int player_run(struct player *player, int argc, char const *argv[])
{
  int result = 0;
  union audio_control control;

  player->playlist.entries = argv;
  player->playlist.length  = argc;

  /* set up terminal settings */

# if defined(USE_TTY) && !defined(_WIN32)
  if ((player->options & PLAYER_OPTION_TTYCONTROL) &&
      setup_tty() == -1)
    player->options &= ~PLAYER_OPTION_TTYCONTROL;
# endif

  /* initialize ancillary data output file */

  if (player->ancillary.path) {
    if (player->output.path &&
	strcmp(player->ancillary.path, player->output.path) == 0) {
      error("output", _("ancillary and audio output have same path"));
      goto fail;
    }

    if (strcmp(player->ancillary.path, "-") == 0)
      player->ancillary.file = stdout;
    else {
      player->ancillary.file = fopen(player->ancillary.path, "wb");
      if (player->ancillary.file == 0) {
	error("ancillary", ":", player->ancillary.path);
	goto fail;
      }
    }
  }

  /* set up filters */

  if (setup_filters(player) == -1) {
    error("filter", _("not enough memory to allocate filters"));
    goto fail;
  }

  set_gain(player, 0, 0);

  /* initialize audio */

  if (player->output.command) {
    audio_control_init(&control, AUDIO_COMMAND_INIT);
    control.init.path = player->output.path;

    if (player->output.command(&control) == -1) {
      error("audio", audio_error, control.init.path);
      goto fail;
    }

    if ((player->options & PLAYER_OPTION_SKIP) &&
	mad_timer_sign(player->global_start) < 0) {
      player->stats.global_timer = player->global_start;

      if (silence(player, mad_timer_abs(player->global_start),
		  _("lead-in")) == -1)
	result = -1;
    }
  }

  /* run playlist */

  if (result == 0)
    result = play_all(player);

  /* drain and close audio */

  if (player->output.command) {
    audio_control_init(&control, AUDIO_COMMAND_FINISH);

    if (player->output.command(&control) == -1) {
      error("audio", audio_error);
      goto fail;
    }
  }

  if (0) {
  fail:
    result = -1;
  }

  /* drain and close ancillary data output file */

  if (player->ancillary.file) {
    if (player->ancillary.length) {
      if (fputc(player->ancillary.buffer << (8 - player->ancillary.length),
		player->ancillary.file) == EOF &&
	  result == 0) {
	error("ancillary", ":fputc");
	result = -1;
      }

      player->ancillary.length = 0;
    }

    if (player->ancillary.file != stdout &&
	fclose(player->ancillary.file) == EOF &&
	result == 0) {
      error("ancillary", ":fclose");
      result = -1;
    }

    player->ancillary.file = 0;
  }

  /* restore terminal settings */

# if defined(USE_TTY) && !defined(_WIN32)
  if (player->options & PLAYER_OPTION_TTYCONTROL)
    restore_tty(0);
# endif

  return result;
}
