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
# include <string.h>
# include <unistd.h>

# ifdef HAVE_ERRNO_H
#  include <errno.h>
# endif

# include <mad.h>

# include "getopt.h"
# include "gettext.h"

# include "audio.h"

struct audio {
  char const *fname;
  FILE *file;
  int active;
  mad_fixed_t scale;

  struct mad_frame frame;
};

char const *argv0;

/*
 * NAME:	error()
 * DESCRIPTION:	show a labeled error message
 */
static
void error(char const *id, char const *format, ...)
{
  int err;
  va_list args;

  err = errno;

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

/*
 * NAME:	do_output()
 * DESCRIPTION:	play mixed output
 */
static
int do_output(int (*audio)(union audio_control *),
	      struct mad_frame *frame, struct mad_synth *synth)
{
  union audio_control control;
  static unsigned int channels;
  static unsigned long speed;

  if (channels != synth->pcm.channels ||
      speed    != synth->pcm.samplerate) {
    audio_control_init(&control, AUDIO_COMMAND_CONFIG);

    control.config.channels = synth->pcm.channels;
    control.config.speed    = synth->pcm.samplerate;

    if (audio(&control) == -1) {
      error("output", audio_error);
      return -1;
    }

    channels = synth->pcm.channels;
    speed    = synth->pcm.samplerate;
  }

  audio_control_init(&control, AUDIO_COMMAND_PLAY);

  control.play.nsamples   = synth->pcm.length;
  control.play.samples[0] = synth->pcm.samples[0];
  control.play.samples[1] = synth->pcm.samples[1];
  control.play.mode       = AUDIO_MODE_DITHER;

  if (audio(&control) == -1) {
    error("output", audio_error);
    return -1;
  }

  return 0;
}

/*
 * NAME:	do_mix()
 * DESCRIPTION:	perform mixing and audio output
 */
static
int do_mix(struct audio *mix, int ninputs, int (*audio)(union audio_control *))
{
  struct mad_frame frame;
  struct mad_synth synth;
  int i, count;

  mad_frame_init(&frame);
  mad_synth_init(&synth);

  count = ninputs;

  while (1) {
    int ch, s, sb;

    mad_frame_mute(&frame);

    for (i = 0; i < ninputs; ++i) {
      if (!mix[i].active)
	continue;

      if (fread(&mix[i].frame, sizeof(mix[i].frame), 1, mix[i].file) != 1) {
	if (ferror(mix[i].file))
	  error("fread", ":", mix[i].fname);
	mix[i].active = 0;
	--count;
	continue;
      }

      mix[i].frame.overlap = 0;

      if (frame.header.layer == 0) {
	frame.header.layer	    = mix[i].frame.header.layer;
	frame.header.mode	    = mix[i].frame.header.mode;
	frame.header.mode_extension = mix[i].frame.header.mode_extension;
	frame.header.emphasis	    = mix[i].frame.header.emphasis;

	frame.header.bitrate	    = mix[i].frame.header.bitrate;
	frame.header.samplerate	    = mix[i].frame.header.samplerate;

	frame.header.flags	    = mix[i].frame.header.flags;
	frame.header.private_bits   = mix[i].frame.header.private_bits;

	frame.header.duration	    = mix[i].frame.header.duration;
      }

      for (ch = 0; ch < 2; ++ch) {
	for (s = 0; s < 36; ++s) {
	  for (sb = 0; sb < 32; ++sb) {
	    frame.sbsample[ch][s][sb] +=
	      mad_f_mul(mix[i].frame.sbsample[ch][s][sb], mix[i].scale);
	  }
	}
      }
    }

    if (count == 0)
      break;

    mad_synth_frame(&synth, &frame);
    do_output(audio, &frame, &synth);
  }

  mad_synth_finish(&synth);
  mad_frame_finish(&frame);

  return 0;
}

/*
 * NAME:	audio->init()
 * DESCRIPTION:	initialize the audio output module
 */
static
int audio_init(int (*audio)(union audio_control *), char const *path)
{
  union audio_control control;

  audio_control_init(&control, AUDIO_COMMAND_INIT);
  control.init.path = path;

  if (audio(&control) == -1) {
    error("audio", audio_error, control.init.path);
    return -1;
  }

  return 0;
}

/*
 * NAME:	audio->finish()
 * DESCRIPTION:	terminate the audio output module
 */
static
int audio_finish(int (*audio)(union audio_control *))
{
  union audio_control control;

  audio_control_init(&control, AUDIO_COMMAND_FINISH);

  if (audio(&control) == -1) {
    error("audio", audio_error);
    return -1;
  }

  return 0;
}

/*
 * NAME:	usage()
 * DESCRIPTION:	display usage message and exit
 */
static
void usage(char const *argv0)
{
  fprintf(stderr, _("Usage: %s input1 [input2 ...]\n"), argv0);
}

/*
 * NAME:	main()
 * DESCRIPTION:	program entry point
 */
int main(int argc, char *argv[])
{
  int opt, ninputs, i, result = 0;
  int (*output)(union audio_control *) = 0;
  char const *fname, *opath = 0;
  FILE *file;
  struct audio *mix;

  argv0 = argv[0];

  if (argc > 1) {
    if (strcmp(argv[1], "--version") == 0) {
      printf("%s - %s\n", mad_version, mad_copyright);
      printf(_("Build options: %s\n"), mad_build);
      return 0;
    }
    if (strcmp(argv[1], "--help") == 0) {
      usage(argv[0]);
      return 0;
    }
  }

  while ((opt = getopt(argc, argv, "o:")) != -1) {
    switch (opt) {
    case 'o':
      opath = optarg;

      output = audio_output(&opath);
      if (output == 0) {
	error(0, _("%s: unknown output format type"), opath);
	return 2;
      }
      break;

    default:
      usage(argv[0]);
      return 1;
    }
  }

  if (optind == argc) {
    usage(argv[0]);
    return 1;
  }

  if (output == 0)
    output = audio_output(0);

  if (audio_init(output, opath) == -1)
    return 2;

  ninputs = argc - optind;

  mix = malloc(ninputs * sizeof(*mix));
  if (mix == 0) {
    error(0, _("not enough memory to allocate mixing buffers"));
    return 3;
  }

  printf(_("mixing %d streams\n"), ninputs);

  for (i = 0; i < ninputs; ++i) {
    if (strcmp(argv[optind + i], "-") == 0) {
      fname = "stdin";
      file  = stdin;
    }
    else {
      fname = argv[optind + i];
      file = fopen(fname, "rb");
      if (file == 0) {
	error(0, ":", fname);
	return 4;
      }
    }

    mix[i].fname  = fname;
    mix[i].file   = file;
    mix[i].active = 1;
    mix[i].scale  = mad_f_tofixed(1.0);  /* / ninputs); */
  }

  if (do_mix(mix, ninputs, output) == -1)
    result = 5;

  for (i = 0; i < ninputs; ++i) {
    file = mix[i].file;

    if (file != stdin) {
      if (fclose(file) == EOF) {
	error(0, ":", mix[i].fname);
	result = 6;
      }
    }
  }

  free(mix);

  if (audio_finish(output) == -1)
    result = 7;

  return result;
}
