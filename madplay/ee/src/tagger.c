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
 * $Id: tagger.c,v 1.6 2004/01/23 09:41:32 rob Exp $
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "global.h"

# include <stdio.h>
# include <id3tag.h>

# include "tagger.h"

void tagger_init(struct tagger *tagger)
{
  tagger->mode = TAGGER_MODE_DISPLAY;
}

void tagger_finish(struct tagger *tagger)
{
}

static
int tag_one(struct tagger *tagger, char const *path)
{
  struct id3_file *file;

  file = id3_file_open(path, tagger->mode == TAGGER_MODE_MODIFY ?
		       ID3_FILE_MODE_READWRITE : ID3_FILE_MODE_READONLY);
  if (file == 0) {
    perror(path);
    return -1;
  }

  /* ... */

  if (id3_file_close(file) == -1) {
    perror(path);
    return -1;
  }

  return 0;
}

int tagger_run(struct tagger *tagger, int argc, char const *argv[])
{
  int i, result = 0;

  for (i = 0; i < argc; ++i) {
    if (tag_one(tagger, argv[i]) == -1)
      result = -1;
  }

  return result;
}
