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

# ifndef RGAIN_H
# define RGAIN_H

# include <mad.h>

# define RGAIN_REFERENCE  83		/* reference level (dB SPL) */

enum rgain_name {
  RGAIN_NAME_NOT_SET    = 0x0,
  RGAIN_NAME_RADIO      = 0x1,
  RGAIN_NAME_AUDIOPHILE = 0x2
};

enum rgain_originator {
  RGAIN_ORIGINATOR_UNSPECIFIED = 0x0,
  RGAIN_ORIGINATOR_PRESET      = 0x1,
  RGAIN_ORIGINATOR_USER        = 0x2,
  RGAIN_ORIGINATOR_AUTOMATIC   = 0x3
};

struct rgain {
  enum rgain_name name;			/* profile (see above) */
  enum rgain_originator originator;	/* source (see above) */
  signed short adjustment;		/* in units of 0.1 dB */
};

# define RGAIN_SET(rgain)	((rgain)->name != RGAIN_NAME_NOT_SET)

# define RGAIN_VALID(rgain)  \
  (((rgain)->name == RGAIN_NAME_RADIO ||  \
    (rgain)->name == RGAIN_NAME_AUDIOPHILE) &&  \
   (rgain)->originator != RGAIN_ORIGINATOR_UNSPECIFIED)

# define RGAIN_DB(rgain)  ((rgain)->adjustment / 10.0)

void rgain_parse(struct rgain *, struct mad_bitptr *);
char const *rgain_originator(struct rgain const *);

# endif
