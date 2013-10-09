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
 * $Id: version.c,v 1.18 2004/01/23 09:41:32 rob Exp $
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "global.h"

# include <stdio.h>
# include <string.h>
# include <mad.h>
# include <id3tag.h>

# include "gettext.h"

# include "version.h"

# define STRINGIZE(str)	#str
# define STRING(str)	STRINGIZE(str)

# define COPYRIGHT	"Copyright (C)"

char const madplay_version[]   = "madplay " MADPLAY_VERSION;
char const madplay_copyright[] = COPYRIGHT " " MADPLAY_PUBLISHYEAR
                                 " " MADPLAY_AUTHOR;
char const madplay_author[]    = MADPLAY_AUTHOR " <" MADPLAY_EMAIL ">";

char const madplay_build[] = ""
# if defined(DEBUG)
  "DEBUG "
# elif defined(NDEBUG)
  "NDEBUG "
# endif

# if defined(EXPERIMENTAL)
  "EXPERIMENTAL "
# endif

  "AUDIO_DEFAULT=" STRING(AUDIO_DEFAULT) " "

# if defined(ENABLE_NLS)
  "ENABLE_NLS "
# endif
;

void ver_banner(FILE *stream)
{
  fprintf(stream, "%s %s - %s %s %s et al.\n", _("MPEG Audio Decoder"),
	  MADPLAY_VERSION,
	  _("Copyright (C)"), MADPLAY_PUBLISHYEAR, MADPLAY_AUTHOR);

  fflush(stream);
}

void copyright(FILE *stream, char const *str)
{
  if (strstr(str, COPYRIGHT) == str)
    fprintf(stream, "  %s%s\n", _(COPYRIGHT), &str[sizeof(COPYRIGHT) - 1]);
  else
    fprintf(stream, "  %s\n", str);
}

void ver_version(FILE *stream)
{
  fprintf(stream, "%s\n", mad_version);
  copyright(stream, mad_copyright);
  fprintf(stream, "  %s: %s\n\n", _("Build options"), mad_build);

  fprintf(stream, "%s\n", id3_version);
  copyright(stream, id3_copyright);
  fprintf(stream, "  %s: %s\n\n", _("Build options"), id3_build);

  fprintf(stream, "%s\n", madplay_version);
  copyright(stream, madplay_copyright);
  fprintf(stream, "  %s: %s\n\n", _("Build options"), madplay_build);
}

void ver_license(FILE *stream)
{
  fputc('\n', stream);
  fprintf(stream,
  _("This program is free software; you can redistribute it and/or modify it\n"
    "under the terms of the GNU General Public License as published by the\n"
    "Free Software Foundation; either version 2 of the License, or (at your\n"
    "option) any later version.\n\n"

    "This program is distributed in the hope that it will be useful, but\n"
    "WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n"
    "General Public License for more details.\n\n"

    "You should have received a copy of the GNU General Public License along\n"
    "with this program; if not, write to the Free Software Foundation, Inc.,\n"
    "59 Temple Place, Suite 330, Boston, MA 02111-1307 USA\n\n"

    "Some portions of this program may be licensable under different terms.\n"
    "To inquire about alternate licensing, contact: %s\n"), madplay_author);

  fputc('\n', stream);
}
