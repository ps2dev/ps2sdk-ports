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
 * $Id: tagger.h,v 1.5 2004/01/23 09:41:32 rob Exp $
 */

# ifndef TAGGER_H
# define TAGGER_H

enum tagger_mode {
  TAGGER_MODE_DISPLAY,
  TAGGER_MODE_MODIFY
};

struct tagger {
  enum tagger_mode mode;
};

void tagger_init(struct tagger *);
void tagger_finish(struct tagger *);

int tagger_run(struct tagger *, int, char const *[]);

# endif
