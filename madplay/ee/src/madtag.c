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
 * $Id: madtag.c,v 1.2 2004/01/23 09:41:32 rob Exp $
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "global.h"

/* include this first to avoid conflicts with MinGW __argc et al. */
# include "getopt.h"

# include <locale.h>
# include <stdio.h>
# include <stdlib.h>

# ifdef HAVE_ASSERT_H
#  include <assert.h>
# endif

# include <id3tag.h>

# include "gettext.h"

# include "tagger.h"

static
struct option const options[] = {
  { "help",		no_argument,		0,		 'h' },
  { "version",		no_argument,		0,		 'V' },
  { 0 }
};

char const *argv0;

# define EPUTS(str)	fputs(_(str), stream)

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

  EPUTS(_("Display or modify ID3 tags in FILE(s).\n"));

  /* the following usage text should agree with the option names */

  EPUTS(_("\nMiscellaneous:\n"));
  EPUTS(_("  -V, --version                display version number and exit\n"));
  EPUTS(_("  -h, --help                   display this help and exit\n"));
}

# undef EPUTS

/*
 * NAME:	get_options()
 * DESCRIPTION:	parse command-line options or die
 */
static
void get_options(int argc, char *argv[], struct tagger *tagger)
{
  int opt, index;

  while ((opt = getopt_long(argc, argv,
			    "Vh",	/* miscellaneous options */
			    options, &index)) != -1) {
    switch (opt) {
    case 0:
      break;

    case 'h':
      show_usage(1);
      exit(0);

    case 'V':
      fprintf(stderr, "%s %s\n  %s %s %s\n",
	      _("ID3 Tag Library"), ID3_VERSION,
	      _("Copyright (C)"), ID3_PUBLISHYEAR, ID3_AUTHOR);
      exit(0);

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

int main(int argc, char *argv[])
{
  struct tagger tagger;
  int result = 0;

  argv0 = argv[0];

  /* internationalization support */

# if defined(ENABLE_NLS)
  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, LOCALEDIR);
  textdomain(PACKAGE);
# endif

  /* initialize and get options */

  tagger_init(&tagger);

  get_options(argc, argv, &tagger);

  /* main processing */

  if (tagger_run(&tagger, argc - optind, (char const **) &argv[optind]) == -1)
    result = 4;

  /* finish up */

  tagger_finish(&tagger);

  return result;
}
