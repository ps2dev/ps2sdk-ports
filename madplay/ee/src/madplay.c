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
 * $Id: madplay.c,v 1.86 2004/02/23 21:34:53 rob Exp $
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "global.h"

/* include this first to avoid conflicts with MinGW __argc et al. */
# include "getopt.h"

# include <locale.h>
# include <stdio.h>
# include <stdarg.h>
# include <stdlib.h>
# include <string.h>

# ifdef HAVE_ASSERT_H
#  include <assert.h>
# endif

# ifdef HAVE_UNISTD_H
#  include <unistd.h>
# endif

# ifdef HAVE_FCNTL_H
#  include <fcntl.h>
# endif

# include <ctype.h>
# include <mad.h>

# include "gettext.h"

# include "version.h"
# include "audio.h"
# include "player.h"

# define FADE_DEFAULT	"0:05"

# if defined(EXPERIMENTAL)
static int external_mix;
static int experimental;
# endif

static
struct option const options[] = {
  { "adjust-volume",	required_argument,	0,		 'A' },
  { "amplify",		required_argument,	0,		 'a' },
  { "ancillary-output",	required_argument,	0,		-'a' },
  { "attenuate",	required_argument,	0,		 'a' },
  { "bit-depth",	required_argument,	0,		 'b' },
  { "display-time",	required_argument,	0,		-'t' },
  { "downsample",	no_argument,		0,		-'d' },
  { "fade-in",		optional_argument,	0,		-'i' },
  { "help",		no_argument,		0,		 'h' },
  { "ignore-crc",	no_argument,		0,		 'i' },
  { "left",		no_argument,		0,		 '1' },
  { "license",		no_argument,		0,		-'l' },
  { "mono",		no_argument,		0,		 'm' },
  { "no-dither",	no_argument,		0,		 'd' },
  { "output",		required_argument,	0,		 'o' },
  { "pre-amp",		required_argument,	0,		 'a' },
  { "quiet",		no_argument,		0,		 'q' },
  { "repeat",		optional_argument,	0,		 'r' },
  { "replay-gain",	optional_argument,	0,		 'G' },
  { "right",		no_argument,		0,		 '2' },
  { "sample-rate",	required_argument,	0,		 'R' },
  { "show-tags-only",	no_argument,		0,		 'T' },
  { "shuffle",		no_argument,		0,		 'z' },
  { "start",		required_argument,	0,		 's' },
  { "stereo",		no_argument,		0,		 'S' },
  { "time",		required_argument,	0,		 't' },
  { "verbose",		no_argument,		0,		 'v' },
  { "version",		no_argument,		0,		 'V' },
  { "very-quiet",	no_argument,		0,		 'Q' },
# if defined(USE_TTY)
  { "tty-control",	no_argument,		0,		-'c' },
  { "no-tty-control",	no_argument,		0,		-'C' },
# endif
# if 0
  { "cross-fade",	no_argument,		0,		 'x' },
  { "fade-out",		optional_argument,	0,		-'o' },
  { "gap",		required_argument,	0,		 'g' },
# endif
# if defined(EXPERIMENTAL)
  { "external-mix",	no_argument,		&external_mix,	  1  },
  { "experimental",	no_argument,		&experimental,	  1  },
# endif
  { 0 }
};

char const *argv0;

# define EPUTS(str)	fputs(str, stream)

/*
 * NAME:	show_usage()
 * DESCRIPTION:	display usage message
 */
static
void show_usage(int verbose)
{
  FILE *stream = verbose ? stdout : stderr;

  fprintf(stream, _("Usage: %s [OPTIONS] FILE [...]\n"), argv0);

  if (!verbose) {
    fprintf(stream, _("Try `%s --help' for more information.\n"), argv0);
    return;
  }

  EPUTS(_("Decode and play MPEG audio FILE(s).\n"));

  /* the following usage text should agree with the option names */

  EPUTS(_("\nVerbosity:\n"));
  EPUTS(_("  -v, --verbose                show status while decoding\n"));
  EPUTS(_("  -q, --quiet                  be quiet but show warnings\n"));
  EPUTS(_("  -Q, --very-quiet             "
	  "be quiet and do not show warnings\n"));
  EPUTS(_("      --display-time=MODE      "
	  "use default verbose time display MODE\n"
	  "                                 (remaining, current, overall)\n"));

  EPUTS(_("\nDecoding:\n"));
  EPUTS(_("      --downsample             reduce sample rate 2:1\n"));
  EPUTS(_("  -i, --ignore-crc             ignore CRC errors\n"));
  EPUTS(_("      --ancillary-output=PATH  write ancillary data to PATH\n"));

  EPUTS(_("\nAudio output:\n"));
  EPUTS(_("  -o, --output=[TYPE:]PATH     "
	  "write output to PATH with format TYPE (below)\n"));
  EPUTS(_("  -b, --bit-depth=DEPTH        request DEPTH bits per sample\n"));
  EPUTS(_("  -R, --sample-rate=HERTZ      "
	  "request HERTZ samples per second\n"));
  EPUTS(_("  -d, --no-dither              "
	  "do not dither output PCM samples\n"));
  fprintf(stream,
	_("      --fade-in[=DURATION]     "
	  "fade-in songs over DURATION (default %s)\n"), FADE_DEFAULT);
# if 0
  fprintf(stream,
	_("      --fade-out[=DURATION]    "
	  "fade-out songs over DURATION (default %s)\n"), FADE_DEFAULT);
  EPUTS(_("  -g, --gap=DURATION           set inter-song gap to DURATION\n"));
  EPUTS(_("  -x, --cross-fade             "
	  "cross-fade songs (use with negative gap)\n"));
# endif
  EPUTS(_("  -a, --attenuate=DECIBELS     "
	  "attenuate signal by DECIBELS (-)\n"));
  EPUTS(_("  -a, --amplify=DECIBELS       amplify signal by DECIBELS (+)\n"));
  EPUTS(_("  -A, --adjust-volume=DECIBELS "
	  "override per-file volume adjustments\n"));
  EPUTS(_("  -G, --replay-gain[=PROFILE]  "
	  "enable Replay Gain volume adjustments using\n"
	  "                                 PROFILE (radio, audiophile)\n"));

  EPUTS(_("\nChannel selection:\n"));
  EPUTS(_("  -1, --left                   "
	  "output first (left) channel only\n"));
  EPUTS(_("  -2, --right                  "
	  "output second (right) channel only\n"));
  EPUTS(_("  -m, --mono                   "
	  "mix left and right channels for monaural output\n"));
  EPUTS(_("  -S, --stereo                 force stereo output\n"));

# if defined(EXPERIMENTAL)
  EPUTS(_("\nExperimental:\n"));
  EPUTS(_("      --external-mix           "
	  "output pre-synthesis samples for external mixer\n"));
  EPUTS(_("      --experimental           enable experimental filter\n"));
# endif

  EPUTS(_("\nPlayback:\n"));
  EPUTS(_("  -s, --start=TIME             "
	  "skip to begin at TIME (HH:MM:SS.DDD)\n"));
  EPUTS(_("  -t, --time=DURATION          "
	  "play only for DURATION (HH:MM:SS.DDD)\n"));
  EPUTS(_("  -z, --shuffle                randomize file list\n"));
  EPUTS(_("  -r, --repeat[=MAX]           "
	  "play files MAX times, or indefinitely\n"));
# if defined(USE_TTY)
  EPUTS(_("      --tty-control            enable keyboard controls\n"));
  EPUTS(_("      --no-tty-control         disable keyboard controls\n"));
# endif

  EPUTS(_("\nMiscellaneous:\n"));
  EPUTS(_("  -T, --show-tags-only         "
	  "show ID3/encoder tags only (do not decode)\n"));
  EPUTS(_("  -V, --version                display version number and exit\n"));
  EPUTS(_("      --license                "
	  "show copyright/license message and exit\n"));
  EPUTS(_("  -h, --help                   display this help and exit\n"));

  EPUTS(_("\nSupported output formats:\n"));
  EPUTS(_("  cdda    "
	  "CD audio, 16-bit big-endian 44100 Hz stereo PCM (*.cdr, *.cda)\n"));
  EPUTS(_("  aiff    Audio IFF, [16-bit] PCM (*.aif, *.aiff)\n"));
  EPUTS(_("  wave    Microsoft RIFF/WAVE, [16-bit] PCM (*.wav)\n"));
  fprintf(stream,
	_("  snd     Sun/NeXT audio, 8-bit ISDN %s (*.au, *.snd)\n"),
	  _("mu-law"));
  EPUTS(_("  raw     binary [16-bit] host-endian linear PCM\n"));
  EPUTS(_("  hex     ASCII hexadecimal [24-bit] linear PCM\n"));
# if defined(HAVE_LIBESD)
  EPUTS(_("  esd     "
	  "Enlightened Sound Daemon [16-bit] (give speaker host as PATH)\n"));
# endif
# if defined(HAVE_LIBAUDIO)
  EPUTS(_("  nas     "
	  "Network Audio System [16-bit] (give server name as PATH)\n"));
# endif
  EPUTS(_("  null    no output (decode only)\n"));
}

# undef EPUTS

/*
 * NAME:	verror()
 * DESCRIPTION:	print error message with program title prefix
 */
static
void verror(char const *message, va_list args)
{
  fprintf(stderr, "%s: ", argv0);
  vfprintf(stderr, message, args);
  fputc('\n', stderr);
}

/*
 * NAME:	warn()
 * DESCRIPTION:	print warning message
 */
static
void warn(char const *message, ...)
{
  va_list args;

  va_start(args, message);
  verror(message, args);
  va_end(args);
}

/*
 * NAME:	die()
 * DESCRIPTION:	exit with failure status after printing message
 */
static
void die(char const *message, ...)
{
  va_list args;

  va_start(args, message);
  verror(message, args);
  va_end(args);

  exit(1);
}

/*
 * NAME:	parse_time()
 * DESCRIPTION:	parse a time specification string
 */
static
int parse_time(mad_timer_t *timer, char const *str)
{
  mad_timer_t time, accum = mad_timer_zero;
  signed long decimal;
  unsigned long seconds, fraction, fracpart;
  int minus;

  while (isspace((unsigned char) *str))
    ++str;

  do {
    seconds = fraction = fracpart = 0;

    switch (*str) {
    case '-':
      ++str;
      minus = 1;
      break;

    case '+':
      ++str;
    default:
      minus = 0;
    }

    do {
      decimal = strtol(str, (char **) &str, 10);
      if (decimal < 0)
	return -1;

      seconds += decimal;

      if (*str == ':') {
	seconds *= 60;
	++str;
      }
    }
    while (*str >= '0' && *str <= '9');

    if (*str == '.'
# if defined(HAVE_LOCALECONV)
	|| *str == *localeconv()->decimal_point
# endif
	) {
      char const *ptr;

      decimal = strtol(++str, (char **) &ptr, 10);
      if (decimal < 0)
	return -1;

      fraction = decimal;

      for (fracpart = 1; str != ptr; ++str)
	fracpart *= 10;
    }
    else if (*str == '/') {
      ++str;

      decimal = strtol(str, (char **) &str, 10);
      if (decimal < 0)
	return -1;

      fraction = seconds;
      fracpart = decimal;

      seconds  = 0;
    }

    mad_timer_set(&time, seconds, fraction, fracpart);
    if (minus)
      mad_timer_negate(&time);

    mad_timer_add(&accum, time);
  }
  while (*str == '-' || *str == '+');

  while (isspace((unsigned char) *str))
    ++str;

  if (*str != 0)
    return -1;

  *timer = accum;

  return 0;
}

/*
 * NAME:	get_time()
 * DESCRIPTION:	parse a time value or die
 */
static
mad_timer_t get_time(char const *str, int positive, char const *name)
{
  mad_timer_t time;

  if (parse_time(&time, str) == -1)
    die(_("invalid %s specification \"%s\""), name, str);

  if (positive && mad_timer_sign(time) <= 0)
    die(_("%s must be positive"), name);

  return time;
}

/*
 * NAME:	parse_decibels()
 * DESCRIPTION:	parse a decibel value specification string
 */
static
int parse_decibels(double *db, char const *str)
{
  *db = strtod(str, (char **) &str);

  while (isspace((unsigned char) *str))
    ++str;

  if (strncasecmp(str, "dB", 2) == 0) {
    str += 2;

    while (isspace((unsigned char) *str))
      ++str;
  }

  if (*str != 0)
    return -1;

  return 0;
}

/*
 * NAME:	get_decibels()
 * DESCRIPTION:	parse a decibel value into a linear ratio or die
 */
static
double get_decibels(char const *str)
{
  double db;

  if (parse_decibels(&db, str) == -1)
    die(_("invalid decibel specification \"%s\""), str);

  if (db < DB_MIN || db > DB_MAX)
    die(_("decibel value must be in the range %+d to %+d dB"), DB_MIN, DB_MAX);

  return db;
}

/*
 * NAME:	parse_hertz()
 * DESCRIPTION:	parse a hertz specification string
 */
static
int parse_hertz(double *hz, char const *str)
{
  *hz = strtod(str, (char **) &str);

  while (isspace((unsigned char) *str))
    ++str;

  if (*str == 'k' || *str == 'K') {
    *hz *= 1000;

    do
      ++str;
    while (isspace((unsigned char) *str));
  }

  if (strncasecmp(str, "Hz", 2) == 0) {
    str += 2;

    while (isspace((unsigned char) *str))
      ++str;
  }

  if (*str != 0)
    return -1;

  return 0;
}

/*
 * NAME:	get_hertz()
 * DESCRIPTION:	parse a hertz value or die
 */
static
unsigned int get_hertz(char const *str)
{
  double hz;

  enum {
    HZ_MIN = 1000,
    HZ_MAX = 0xffff
  };

  if (parse_hertz(&hz, str) == -1)
    die(_("invalid hertz specification \"%s\""), str);

  if (hz < HZ_MIN || hz > HZ_MAX)
    die(_("hertz value must be in the range %u to %u Hz"), HZ_MIN, HZ_MAX);

  return (unsigned int) (hz + 0.5);
}

/*
 * NAME:	get_options()
 * DESCRIPTION:	parse command-line options or die
 */
static
void get_options(int argc, char *argv[], struct player *player)
{
  int opt, index;
  int ttyset = 0, preamp = 0;

  while ((opt = getopt_long(argc, argv,
			    "vqQ"		/* verbosity options */
			    "i"			/* decoding options */
			    "o:b:R:da:A:G::"	/* audio output options */
# if 0
			    "g:x"
# endif
			    "12mS"		/* channel selection options */
			    "s:t:zr::"		/* playback options */
			    "TVh",		/* miscellaneous options */
			    options, &index)) != -1) {
    switch (opt) {
    case 0:
      break;

    case '1':
    case '2':
      player->output.select = PLAYER_CHANNEL_LEFT + (opt - '1');
      break;

    case 'a':
      player->output.attamp_db = get_decibels(optarg);
      preamp = 1;
      break;

    case 'A':
      player->output.voladj_db = get_decibels(optarg);
      player->options |= PLAYER_OPTION_IGNOREVOLADJ;
      break;

    case -'a':
      if (player->ancillary.path)
	die(_("multiple output destinations not supported"));

      player->ancillary.path = optarg;
      break;

    case 'b':
      opt = atoi(optarg);
      if (opt <= 0)
	die(_("invalid bit depth \"%s\""), optarg);

      player->output.precision_in = opt;
      break;

# if defined(USE_TTY)
    case -'c':
      player->options |= PLAYER_OPTION_TTYCONTROL;
      ttyset = 1;
      break;

    case -'C':
      player->options &= ~PLAYER_OPTION_TTYCONTROL;
      ttyset = 1;
      break;
# endif

    case 'd':
      player->output.mode = AUDIO_MODE_ROUND;
      break;

    case -'d':
      player->options |= PLAYER_OPTION_DOWNSAMPLE;
      break;

# if 0
    case 'g':
      player->gap = get_time(optarg, 0, _("gap time"));
      player->options |= PLAYER_OPTION_GAP;
      break;
# endif

    case 'G':
      player->output.replay_gain |= PLAYER_RGAIN_ENABLED;

      if (optarg) {
	size_t len;

	len = strlen(optarg);

	if (len && strncmp(optarg, "audiophile", len) == 0)
	  player->output.replay_gain |= PLAYER_RGAIN_AUDIOPHILE;
	else if (len && strncmp(optarg, "radio", len) == 0)
	  ;
	else
	  die(_("invalid Replay Gain argument \"%s\""), optarg);
      }

      if (!preamp)
	player->output.attamp_db = +6;  /* default pre-amp */
      break;

    case 'h':
      show_usage(1);
      exit(0);

    case 'i':
      player->options |= PLAYER_OPTION_IGNORECRC;
      break;

    case -'i':
      player->fade_in = get_time(optarg ? optarg : FADE_DEFAULT, 1,
				 _("fade-in time"));
      player->options |= PLAYER_OPTION_FADEIN;
      break;

    case -'l':
      ver_license(stdout);
      exit(0);

    case 'm':
      player->output.select = PLAYER_CHANNEL_MONO;
      break;

    case 'o':
      if (player->output.path)
	die(_("multiple output destinations not supported"));

      player->output.path = optarg;

      player->output.command = audio_output(&player->output.path);
      if (player->output.command == 0)
	die(_("unknown output format type for \"%s\""), optarg);

      if (!ttyset)
	player->options &= ~PLAYER_OPTION_TTYCONTROL;
      break;

# if 0
    case -'o':
      player->fade_out = get_time(optarg ? optarg : FADE_DEFAULT, 1,
				  _("fade-out time"));
      player->options |= PLAYER_OPTION_FADEOUT;
      break;
# endif

    case 'q':
      player->verbosity = -1;

      if (!ttyset)
	player->options &= ~PLAYER_OPTION_TTYCONTROL;
      break;

    case 'Q':
      player->verbosity = -2;

      if (!ttyset)
	player->options &= ~PLAYER_OPTION_TTYCONTROL;
      break;

    case 'r':
      if (optarg == 0)
	player->repeat = -1;
      else {
	player->repeat = atoi(optarg);
	if (player->repeat <= 0)
	  die(_("invalid repeat count \"%s\""), optarg);
      }
      break;

    case 'R':
      player->output.speed_request = get_hertz(optarg);
      break;

    case 's':
      player->global_start = get_time(optarg, 0, _("start time"));
      player->options |= PLAYER_OPTION_SKIP;
      break;

    case 'S':
      player->output.select = PLAYER_CHANNEL_STEREO;
      break;

    case 't':
      player->global_stop = get_time(optarg, 1, _("playing time"));
      player->options |= PLAYER_OPTION_TIMED;
      break;

    case -'t':
      {
	size_t len;

	len = strlen(optarg);

	if (len && strncmp(optarg, "remaining", len) == 0)
	  player->stats.show = STATS_SHOW_REMAINING;
	else if (len && strncmp(optarg, "current", len) == 0)
	  player->stats.show = STATS_SHOW_CURRENT;
	else if (len && strncmp(optarg, "overall", len) == 0)
	  player->stats.show = STATS_SHOW_OVERALL;
	else
	  die(_("invalid display time argument \"%s\""), optarg);
      }
      break;

    case 'T':
      player->options |= PLAYER_OPTION_SHOWTAGSONLY;
      break;

    case 'v':
      player->verbosity = +1;
      break;

    case 'V':
      ver_version(stdout);
      fprintf(stderr, "`%s --license' %s.\n", argv0,
	      _("for license and warranty information"));
      exit(0);

# if 0
    case 'x':
      player->options |= PLAYER_OPTION_CROSSFADE;
      break;
# endif

    case 'z':
      player->options |= PLAYER_OPTION_SHUFFLE;
      break;

    case '?':
      show_usage(0);
      exit(1);

    default:
      assert(!"option handler");
    }
  }

  if (optind == argc) {
    show_usage(0);
    exit(2);
  }
}

/*
 * NAME:	main()
 * DESCRIPTION:	program entry point
 */
int main(int argc, char *argv[])
{
  struct player player;
  int result = 0;

  argv0 = argv[0];

  /* ensure binary standard I/O */

# if defined(_WIN32)
  _setmode(_fileno(stdin),  _O_BINARY);
  _setmode(_fileno(stdout), _O_BINARY);
# endif

  /* internationalization support */

# if defined(ENABLE_NLS)
  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, LOCALEDIR);
  textdomain(PACKAGE);
# endif

  /* initialize and get options */

  player_init(&player);

# if !defined(__CYGWIN__)  /* Cygwin support for this is currently buggy */
  /* check for default tty control */
  if (isatty(STDIN_FILENO))
    player.options |= PLAYER_OPTION_TTYCONTROL;
# endif

  get_options(argc, argv, &player);

  /* main processing */

  if (player.verbosity >= 0)
    ver_banner(stderr);

  if (player.options & PLAYER_OPTION_CROSSFADE) {
    if (!(player.options & PLAYER_OPTION_GAP))
      warn(_("cross-fade ignored without gap"));
    else if (mad_timer_sign(player.gap) >= 0)
      warn(_("cross-fade ignored without negative gap"));
  }

  if (player.output.replay_gain & PLAYER_RGAIN_ENABLED) {
    if (player.options & PLAYER_OPTION_IGNOREVOLADJ)
      warn(_("volume adjustment ignored with Replay Gain enabled"));
    else
      player.options |= PLAYER_OPTION_IGNOREVOLADJ;
  }

  if ((player.options & PLAYER_OPTION_SHOWTAGSONLY) &&
      player.repeat != 1) {
    warn(_("ignoring repeat"));
    player.repeat = 1;
  }

  /* make stop time absolute */
  if (player.options & PLAYER_OPTION_TIMED)
    mad_timer_add(&player.global_stop, player.global_start);

  /* get default audio output module */
  if (player.output.command == 0 &&
      !(player.options & PLAYER_OPTION_SHOWTAGSONLY))
    player.output.command = audio_output(0);

# if defined(EXPERIMENTAL)
  if (external_mix) {
    player.options |= PLAYER_OPTION_EXTERNALMIX;
    player.output.command = 0;
  }
  if (experimental)
    player.options |= PLAYER_OPTION_EXPERIMENTAL;
# endif

  /* run the player */

  if (player_run(&player, argc - optind, (char const **) &argv[optind]) == -1)
    result = 4;

  /* finish up */

  player_finish(&player);

  return result;
}
